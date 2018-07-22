#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <gmodule.h>

#include "state.h"
#include "execution.h"

typedef enum {
    NonVolatile = 0,
    Volatile = 1
} volatility_t;

static void
set_volatile (matrix_t *m, coord_t c) {
    assert(get_voxel(m, c) == NonVolatile);
    set_voxel(m, c, Volatile);
}

static void
set_volatile_region (matrix_t *m, region_t r) {
    FOR_EACH_COORD(c, r) {
        set_volatile(m, c);
    } END_FOR_EACH_COORD;
}

static int
find_fusion (state_t *state, command_t *commands, command_type_t type, coord_t c) {
    for (int i = 0; i < state->n_bots; i++) {
        if (commands[i].type != type) continue;
        if (!is_coords_equal(state->bots[i].pos, c)) continue;
        return i;
    }
    assert(false);
}

static int
region_dimension(region_t r) {
    int dim = 0;
    dim += (r.c_min.x == r.c_max.x) ? 1 : 0;
    dim += (r.c_min.x == r.c_max.x) ? 1 : 0;
    dim += (r.c_min.x == r.c_max.x) ? 1 : 0;
    return dim;
}

static region_t
make_group_command_region(coord_t bot_pos, command_t cmd) {
    assert(cmd.type == GVoid || cmd.type == GFill);
    assert(&cmd.GVoid_nd == &cmd.GVoid_nd);
    assert(&cmd.GVoid_fd == &cmd.GVoid_fd);

    coord_t near_corner = add_coords(bot_pos, cmd.GVoid_nd);
    coord_t far_corner = add_coords(near_corner, cmd.GVoid_fd);
    return make_region(near_corner, far_corner);
}

void
check_group_bots(state_t *state, command_t *commands, command_type_t type, region_t r) {
    int bots_in_group_command = 0;
    for (int i = 0; i < state->n_bots; i++) {
        if (commands[i].type != type) continue;
        region_t rp = make_group_command_region(state->bots[i].pos, commands[i]);
        if (!region_equal(&r, &rp)) {
            continue;            
        }
        bots_in_group_command++;
    }
    int dim = region_dimension(r);
    int expected_len = powl(2, dim);
    assert(bots_in_group_command == expected_len);
}

execution_t
start_timestep (state_t state, command_t *commands) {
    energy_t res = state.matrix.resolution;
    execution_t exec;

    exec.state = state;
    exec.commands = commands;

    // FIXME: This is highly inefficient
    exec.vol = make_matrix(res);
    exec.new_bots = g_array_new(FALSE, FALSE, sizeof(bot_t));

    exec.new_state = make_state(state.energy, state.harmonics, state.matrix);

    exec.new_state.energy += timestep_energy(&state);
    exec.new_state.energy += bot_energy(&state);

    return exec;
}

state_t
finish_timestep (execution_t *exec) {
    free_matrix(&exec->vol);

    g_array_sort(exec->new_bots, compare_bots);

    exec->new_state.n_bots = exec->new_bots->len;
    exec->new_state.bots = (bot_t*)g_array_free(exec->new_bots, FALSE);

    return exec->new_state;
}

static void
exec_bot (execution_t *exec, bot_t bot, command_t cmd) {
    coord_t c = bot.pos;

    switch (cmd.type) {
    case Halt:
        assert(c.x == 0 && c.y == 0 && c.z == 0);
        assert(exec->state.n_bots == 1);
        assert(exec->state.harmonics == Low);
        set_volatile(&exec->vol, c);
        break;

    case Wait:
        set_volatile(&exec->vol, c);
        g_array_append_val(exec->new_bots, bot);
        break;

    case Flip:
        if (exec->new_state.harmonics == High) {
            exec->new_state.harmonics = Low;
        } else if (exec->new_state.harmonics == Low) {
            exec->new_state.harmonics = High;
        } else {
            assert(false);
        }

        set_volatile(&exec->vol, c);
        g_array_append_val(exec->new_bots, bot);
        break;

    case SMove: {
        coord_t lld = cmd.SMove_lld;
        coord_t cp = add_coords(c, lld);
        assert(is_coord_valid(&exec->state.matrix, cp));
        region_t r = make_region(c, cp);
        assert(region_is_empty(&exec->state.matrix, r));
        set_volatile_region(&exec->vol, r);
        bot.pos = cp;
        g_array_append_val(exec->new_bots, bot);

        exec->new_state.energy += smove_energy(lld);
        break;
    }

    case LMove: {
        coord_t sld1 = cmd.LMove_sld1;
        coord_t sld2 = cmd.LMove_sld2;
        coord_t cp = add_coords(c, sld1);
        coord_t cpp = add_coords(cp, sld2);
        region_t r1 = make_region(c, cp);
        region_t r2 = make_region(cp, cpp);

        assert(is_coord_valid(&exec->state.matrix, cp));
        assert(is_coord_valid(&exec->state.matrix, cpp));
        assert(region_is_empty(&exec->state.matrix, r1));
        set_volatile_region(&exec->vol, r1);
        // cp is set by both regions, so we unset it here
        set_voxel(&exec->vol, cp, NonVolatile);
        assert(region_is_empty(&exec->state.matrix, r2));
        set_volatile_region(&exec->vol, r2);
        bot.pos = cpp;
        g_array_append_val(exec->new_bots, bot);

        exec->new_state.energy += lmove_energy(sld1, sld2);
        break;
    }

    case Fission: {
        assert(bot.n_seeds > 0);
        coord_t cp = add_coords(c, cmd.Fission_nd);

        set_volatile(&exec->vol, c);
        assert(is_coord_valid(&exec->state.matrix, cp));
        set_volatile(&exec->vol, cp);
        assert(get_voxel(&exec->state.matrix, cp) == Empty);

        uint8_t m = cmd.m;
        bot_t botp = make_bot(bot.seeds[0], cp, m, bot.seeds + 1);
        bot = make_bot(bot.bid, bot.pos, bot.n_seeds - 1 - m, bot.seeds + 1 + m);

        g_array_append_val(exec->new_bots, bot);
        g_array_append_val(exec->new_bots, botp);

        exec->new_state.energy += fission_energy();
        break;
    }

    case Fill: {
        coord_t cp = add_coords(c, cmd.Fill_nd);

        set_volatile(&exec->vol, c);
        assert(is_coord_valid(&exec->state.matrix, cp));
        set_volatile(&exec->vol, cp);

        voxel_t v = get_voxel(&exec->state.matrix, cp);
        set_voxel(&exec->state.matrix, cp, Full);
        exec->new_state.energy += fill_energy(v == Empty);

        g_array_append_val(exec->new_bots, bot);
        break;
    }

    case Void: {
        coord_t cp = add_coords(c, cmd.Void_nd);

        set_volatile(&exec->vol, c);
        assert(is_coord_valid(&exec->state.matrix, cp));
        set_volatile(&exec->vol, cp);

        voxel_t v = get_voxel(&exec->state.matrix, cp);
        set_voxel(&exec->state.matrix, cp, Empty);
        exec->new_state.energy += void_energy(v == Full);

        g_array_append_val(exec->new_bots, bot);
        break;
    }

    case FusionP: {
        coord_t cp = add_coords(c, cmd.FusionP_nd);
        set_volatile(&exec->vol, c);
        assert(is_coord_valid(&exec->state.matrix, cp));
        set_volatile(&exec->vol, cp);
        int j = find_fusion(&exec->state, exec->commands, FusionS, cp);
        bot_t bot_s = exec->state.bots[j];
        bot_t new_bot = make_bot(bot.bid, bot.pos, bot.n_seeds + 1 + bot_s.n_seeds, NULL);
        memcpy(new_bot.seeds, bot.seeds, sizeof(bid_t) * bot.n_seeds);
        new_bot.seeds[bot.n_seeds] = bot_s.bid;
        memcpy(new_bot.seeds + bot.n_seeds + 1, bot_s.seeds, sizeof(bid_t) * bot_s.n_seeds);
        qsort(new_bot.seeds, new_bot.n_seeds, sizeof(bid_t), compare_bids);
        g_array_append_val(exec->new_bots, new_bot);

        exec->new_state.energy -= fusion_energy();
        break;
    }

    case FusionS: {
        coord_t cp = add_coords(c, cmd.FusionS_nd);
        assert(is_coord_valid(&exec->state.matrix, cp));
        find_fusion(&exec->state, exec->commands, FusionP, cp);
        break;
    }

    case GVoid: {
        set_volatile(&exec->vol, c);
        
        coord_t near_corner = add_coords(c, cmd.GVoid_nd);
        coord_t far_corner = add_coords(near_corner, cmd.GVoid_fd);
        region_t r = make_region(near_corner, far_corner);
        if (is_coords_equal(near_corner, r.c_min)) {
            check_group_bots(&exec->state, exec->commands, GVoid, r);
            FOR_EACH_COORD(cp, r) {
                set_volatile(&exec->vol, cp);
                voxel_t v = get_voxel(&exec->state.matrix, cp);
                set_voxel(&exec->state.matrix, cp, Empty);
                exec->new_state.energy += void_energy(v == Full);
             } END_FOR_EACH_COORD;
        } 
        g_array_append_val(exec->new_bots, bot);
        break;
    }

    case GFill: {
        set_volatile(&exec->vol, c);
        
        coord_t near_corner = add_coords(c, cmd.GVoid_nd);
        coord_t far_corner = add_coords(near_corner, cmd.GVoid_fd);
        region_t r = make_region(near_corner, far_corner);
        if (is_coords_equal(near_corner, r.c_min)) {
            check_group_bots(&exec->state, exec->commands, GVoid, r);
            FOR_EACH_COORD(cp, r) {
                set_volatile(&exec->vol, cp);
                voxel_t v = get_voxel(&exec->state.matrix, cp);
                set_voxel(&exec->state.matrix, cp, Full);
                exec->new_state.energy += fill_energy(v == Full);
             } END_FOR_EACH_COORD;
        } 
        g_array_append_val(exec->new_bots, bot);
        break;
    }

    default:
        assert(false);
    }
}

state_t
exec_timestep (state_t state, command_t *commands) {
    execution_t exec = start_timestep(state, commands);

    for (int i = 0; i < state.n_bots; i++) {
        bot_t bot = state.bots[i];
        exec_bot(&exec, state.bots[i], commands[i]);
    }

    return finish_timestep(&exec);
}

matrix_t
exec_trace_with_state(trace_t trace, state_t state) {
    int cmd_index = 0;

    while (state.n_bots > 0) {
        int n_bots = state.n_bots;
        // printf("exec %d with %d bots\n", cmd_index, state.n_bots);
        for (int i = 0; i < state.n_bots; i++) {
            // print_cmd(trace.commands[cmd_index + i]);
        }
        assert(cmd_index + state.n_bots <= trace.n_commands);
        state = exec_timestep(state, trace.commands + cmd_index);
        cmd_index += n_bots;
    }

    printf("%lld\n", state.energy);

    return state.matrix;
}

matrix_t
exec_lightning_trace (trace_t trace, resolution_t resolution) {
    state_t state = make_lightning_start_state(resolution);
    return exec_trace_with_state(trace, state);
}

matrix_t exec_trace(trace_t trace, matrix_t src_model, task_mode_t mode) {
    state_t state = make_start_state_from_matrix(src_model, mode);
    return exec_trace_with_state(trace, state);
}

