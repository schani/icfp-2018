#include <assert.h>
#include <stdbool.h>

#include <gmodule.h>

#include "state.h"

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

// FIXME: make public
static bool
coords_equal (coord_t c1, coord_t c2) {
    return c1.x == c2.x && c1.y == c2.y && c1.z == c2.z;
}

static int
find_fusion (state_t *state, timestep_t ts, command_type_t type, coord_t c) {
    for (int i = 0; i < ts.n_commands; i++) {
        if (ts.commands[i].type != type) continue;
        if (!coords_equal(state->bots[i].pos, c)) continue;
        return i;
    }
    assert(false);
}

static int
compare_bids (const void *p1, const void *p2) {
    bid_t b1 = *(const bid_t*)p1;
    bid_t b2 = *(const bid_t*)p2;
    if (b1 < b2) return -1;
    if (b1 == b2) return 0;
    return 1;
}

static int
compare_bots (const void *p1, const void *p2) {
    const bot_t *b1 = (const bot_t*)p1;
    const bot_t *b2 = (const bot_t*)p2;
    return compare_bids(&b1->bid, &b2->bid);
}

static state_t
exec_timestep (state_t *state, timestep_t ts) {
    assert(state->n_bots == ts.n_commands);

    matrix_t vol = make_matrix(state->matrix.resolution);
    GArray *new_bots = g_array_new(FALSE, FALSE, sizeof(bot_t));

    state_t new_state = make_state();
    new_state.energy = state->energy;
    new_state.harmonics = state->harmonics;
    new_state.matrix = state->matrix;

    for (int i = 0; i < ts.n_commands; i++) {
        bot_t bot = state->bots[i];
        command_t *cmd = &ts.commands[i];
        coord_t c = bot.pos;

        switch (cmd->type) {
        case Halt:
            assert(c.x == 0 && c.y == 0 && c.z == 0);
            assert(state->n_bots == 1);
            assert(state->harmonics == Low);
            set_volatile(&vol, c);
            break;

        case Wait:
            set_volatile(&vol, c);
            g_array_append_val(new_bots, bot);
            break;

        case Flip:
            if (new_state.harmonics == High) {
                new_state.harmonics = Low;
            } else if (new_state.harmonics == Low) {
                new_state.harmonics = High;
            } else {
                assert(false);
            }

            set_volatile(&vol, c);
            g_array_append_val(new_bots, bot);
            break;

        case SMove: {
            coord_t cp = add_coords(c, cmd->SMove_lld);
            assert(is_coord_valid(&state->matrix, cp));
            region_t r = make_region(c, cp);
            assert(region_is_empty(&state->matrix, r));
            set_volatile_region(&vol, r);
            bot.pos = cp;
            g_array_append_val(new_bots, bot);
            break;
        }

        case LMove: {
            coord_t cp = add_coords(c, cmd->LMove_sld1);
            coord_t cpp = add_coords(cp, cmd->LMove_sld2);
            region_t r1 = make_region(c, cp);
            region_t r2 = make_region(cp, cpp);

            assert(is_coord_valid(&state->matrix, cp));
            assert(is_coord_valid(&state->matrix, cpp));
            assert(region_is_empty(&state->matrix, r1));
            set_volatile_region(&vol, r1);
            assert(region_is_empty(&state->matrix, r2));
            set_volatile_region(&vol, r2);
            bot.pos = cpp;
            g_array_append_val(new_bots, bot);
            break;
        }

        case Fission: {
            assert(bot.n_seeds > 0);
            coord_t cp = add_coords(c, cmd->Fission_nd);

            set_volatile(&vol, c);
            assert(is_coord_valid(&state->matrix, cp));
            set_volatile(&vol, cp);
            assert(get_voxel(&state->matrix, cp) == Empty);

            uint8_t m = cmd->m;
            bot_t botp = make_bot(bot.seeds[0], cp, m, bot.seeds + 1);
            bot = make_bot(bot.bid, bot.pos, bot.n_seeds - 1 - m, bot.seeds + 1 + m);

            g_array_append_val(new_bots, bot);
            g_array_append_val(new_bots, botp);
            break;
        }

        case Fill: {
            coord_t cp = add_coords(c, cmd->Fill_nd);

            set_volatile(&vol, c);
            assert(is_coord_valid(&state->matrix, cp));
            set_volatile(&vol, cp);

            voxel_t v = get_voxel(&state->matrix, cp);
            if (v == Empty) {
                set_voxel(&state->matrix, cp, Full);
            } else if (v == Full) {
                // count energy
            } else {
                assert(false);
            }
            break;
        }

        case FusionP: {
            coord_t cp = add_coords(c, cmd->FusionP_nd);
            set_volatile(&vol, c);
            assert(is_coord_valid(&state->matrix, cp));
            set_volatile(&vol, cp);
            int j = find_fusion(state, ts, FusionS, cp);
            bot_t bot_s = state->bots[j];
            bot_t new_bot = make_bot(bot.bid, bot.pos, bot.n_seeds + 1 + bot_s.n_seeds, NULL);
            memcpy(new_bot.seeds, bot.seeds, sizeof(bid_t) * bot.n_seeds);
            new_bot.seeds[bot.n_seeds] = bot_s.bid;
            memcpy(new_bot.seeds + bot.n_seeds + 1, bot_s.seeds, sizeof(bid_t) * bot_s.n_seeds);
            qsort(new_bot.seeds, new_bot.n_seeds, sizeof(bid_t), compare_bids);
            g_array_append_val(new_bots, new_bot);
            break;
        }

        case FusionS: {
            coord_t cp = add_coords(c, cmd->FusionS_nd);
            assert(is_coord_valid(&state->matrix, cp));
            find_fusion(state, ts, FusionP, cp);
            break;
        }

        default:
            assert(false);
        }
    }

    free_matrix(vol);

    g_array_sort(new_bots, compare_bots);

    new_state.n_bots = new_bots->len;
    new_state.bots = (bot_t*)g_array_free(new_bots, FALSE);

    return new_state;
}
