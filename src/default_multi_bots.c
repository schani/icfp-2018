#include "default_trace.h"

#include "execution.h"
#include "model.h"
#include "move_helper.h"
#include "multi_bot_helpers.h"
#include "strategy_basic.h"
#include "state.h"

#include <stdio.h>
#include <stdlib.h>


static inline
coord_t above(coord_t c) {
    return add_coords(c, create_coord(0, 1, 0));
}


static void
maybe_fill(matrix_t* m, bot_commands_t* bc, coord_t c) {
    if (get_voxel(m, c) == Full) {
        move_bot_in_multibot_setting(bc, sub_coords(above(c), get_bot_pos(bc)));
        add_cmd(bc->cmds, fill_cmd(create_coord(0, -1, 0)));
    }
}

static void
fill_one_region(matrix_t* m, bot_commands_t* bc, region_t r) {
    move_bot_in_multibot_setting(bc, sub_coords(above(r.c_min), get_bot_pos(bc)));

    for (int y = r.c_min.y; y <= r.c_max.y; y++) {
        for (int x = r.c_min.x; x <= r.c_max.x; x++) {
            for (int z = r.c_min.z; z <= r.c_max.z; z++){
                maybe_fill(m, bc, create_coord(x, y, z));
            }
            if (x < r.c_max.x) {
                x++;
                for (int z = r.c_max.z; z >= r.c_min.z; z--){
                    maybe_fill(m, bc, create_coord(x, y, z));
                }
            }
        }
    }
}

static void
printreg(char* prefix, int bid, region_t r) {
    fprintf(stderr, "%s %d: %d %d %d %d\n", prefix, bid, r.c_min.x, r.c_min.z, r.c_max.x, r.c_max.z);
}

static multi_bot_commands_t
split_up_bots(region_grid_t* r, bot_commands_t initial_bot, bool reserve_bot0) {
    int n_regions = r->rows * r->cols;
    int n_bots = n_regions + (reserve_bot0 ? 1 : 0);

    assert(n_bots<=40);

    multi_bot_commands_t mbc = make_multi_bot_commands(n_bots);
    mbc.bot_commands[0] = initial_bot;

    int first_bot = 0; 

    if (reserve_bot0) {
	first_bot = 1;
        mbc.bot_commands[first_bot] = fission_with_m(&mbc.bot_commands[0], create_coord(0, 0, 1), n_bots-1);
    }
    //move_bot_in_multibot_setting(&mbc.bot_commands[first_bot], r->regions[0].c_min);
    for (int i=1; i < n_regions; i++) {
	int new_bot = first_bot+i;
        mbc.bot_commands[new_bot] = fission_with_m(&mbc.bot_commands[first_bot], create_coord(0, 1, 0), 0);
	add_cmd(mbc.bot_commands[first_bot].cmds, wait_cmd());
 
	region_t new_bot_region = r->regions[n_regions-i];
	coord_t new_bot_start_pos = create_coord(new_bot_region.c_min.x, get_bot_pos(&mbc.bot_commands[new_bot]).y, new_bot_region.c_min.z);

        move_bot_in_multibot_setting(&mbc.bot_commands[new_bot], sub_coords(new_bot_start_pos, get_bot_pos(&mbc.bot_commands[new_bot])));
    }
    return mbc;
}

void
gather_grid_bots(multi_bot_commands_t* mbc, int safe_height, int first_bot) {
    move_bot_in_multibot_setting(&mbc->bot_commands[first_bot], sub_coords(create_coord(0,safe_height,0), get_bot_pos(&mbc->bot_commands[first_bot])));
    move_bot_in_multibot_setting(&mbc->bot_commands[first_bot], create_coord(0, -1, 0));
    for (int i=first_bot+1; i < mbc->n_bots; i++) {
	equalize_some_multi_bot_commands(*mbc, i+1);
	coord_t bp = get_bot_pos(&mbc->bot_commands[i]);
    	move_bot_in_multibot_setting(&mbc->bot_commands[i], create_coord(0, safe_height - bp.y, 0));
    }


    coord_t merging_coords = above(get_bot_pos(&mbc->bot_commands[first_bot]));

    for (int b = mbc->n_bots - 1; b > first_bot; b--) {
        move_bot_in_multibot_setting(&mbc->bot_commands[b], sub_coords(merging_coords, get_bot_pos(&mbc->bot_commands[b])));
	equalize_some_multi_bot_commands(*mbc, b+1);
        fusion(&mbc->bot_commands[first_bot], &mbc->bot_commands[b]);
    }
}


void
move_bots_to_grid(region_grid_t *r, multi_bot_commands_t *mbc){
  //    move_bot_in_multibot_setting(&mbc->bot_commands[0], r->regions[0].c_min);
    move_bot_in_multibot_setting(&mbc->bot_commands[0], sub_coords(r->regions[0].c_min, get_bot_pos(&mbc->bot_commands[0])));
    for (int i=1; i<4; i++) {
        move_bot_in_multibot_setting(&mbc->bot_commands[i], (sub_coords(r->regions[4-i].c_min, get_bot_pos(&mbc->bot_commands[i]))));
        equalize_multi_bot_commands(*mbc);    
    }
    equalize_multi_bot_commands(*mbc);
}


void
calc_multi_bot_constructor_trace(matrix_t *m, multi_bot_commands_t *mbc) {

    region_t rf = matrix_region(m);

    int cols = 2;
    int rows = 2;
    region_grid_t grid = split_region(rf, rows, cols);


    equalize_multi_bot_commands(*mbc);
    
    move_bots_to_grid(&grid, mbc);
    //move_bot_in_multibot_setting(&mbc->bot_commands[0], above(sub_coords(grid.regions[0].c_min, get_bot_pos(&mbc->bot_commands[0]))));
    equalize_multi_bot_commands(*mbc);

    fill_one_region(m, &mbc->bot_commands[0], get_grid_region(grid, 0, 0));
    for (int i=1; i < mbc->n_bots; i++) {
        fill_one_region(m, &mbc->bot_commands[i], grid.regions[mbc->n_bots-i]);
    }

    equalize_multi_bot_commands(*mbc);

    gather_grid_bots(mbc, m->resolution-1, 0);

    move_bot_in_multibot_setting(&mbc->bot_commands[0], sub_coords(create_coord(0,get_bot_pos(&mbc->bot_commands[0]).y,0), get_bot_pos(&mbc->bot_commands[0])));
    move_bot_in_multibot_setting(&mbc->bot_commands[0], sub_coords(create_coord(0,0,0), get_bot_pos(&mbc->bot_commands[0])));

    add_cmd(mbc->bot_commands[0].cmds, flip_cmd());
    add_cmd(mbc->bot_commands[0].cmds, halt_cmd());
}


typedef enum {
    MaybeGrounded = Full,
    SurelyGrounded 
} grounded_t;


static inline
bool is_valid_and_surely_grounded(matrix_t* m, coord_t c) {
    return is_coord_valid(m, c) &&
        (get_voxel(m, c) == SurelyGrounded);
}

// returns true if the voxel at c changed state
// also floods maybegrounded with grounded on change
bool
maybe_set_grounded(matrix_t* m, coord_t c) {
    if (!is_coord_valid(m, c)) return 0;
    voxel_t v = get_voxel(m, c);
    if (v == SurelyGrounded || v == Empty) {
	return 0;
    }
    // v is MaybeGrounded
    if ((c.y == 0) ||
        is_valid_and_surely_grounded(m, add_x(c,  1)) ||
        is_valid_and_surely_grounded(m, add_x(c, -1)) ||
        is_valid_and_surely_grounded(m, add_z(c,  1)) ||
        is_valid_and_surely_grounded(m, add_z(c, -1)) ||
        is_valid_and_surely_grounded(m, add_y(c,  1)) ||
        is_valid_and_surely_grounded(m, add_y(c, -1))) {
	set_voxel(m, c, SurelyGrounded);
        return 1 + 
        maybe_set_grounded(m, add_x(c,  1)) +
        maybe_set_grounded(m, add_x(c, -1)) +
        maybe_set_grounded(m, add_z(c,  1)) +
        maybe_set_grounded(m, add_z(c, -1)) +
        maybe_set_grounded(m, add_y(c,  1)) +
        maybe_set_grounded(m, add_y(c, -1)) +
        0;
    }
    return 0;
}

static bool
has_ungrounded_voxels(matrix_t* m) {
    region_t r = matrix_region(m);
    int ungrounded_voxels;
    bool recount;
    int total_grounded = 0;
    int counts = 0;
    do {
        ungrounded_voxels = 0;
        recount = false;
        counts++;
        FOR_EACH_COORD(c, r) {
            if (get_voxel(m, c) == MaybeGrounded) {
		int grounded = maybe_set_grounded(m ,c);
                total_grounded += grounded;
                if (grounded > 0) {
	            recount = true;
                } else {
                    ungrounded_voxels++;
                }
            }
        } END_FOR_EACH_COORD;
    } while(recount && ungrounded_voxels > 0);
    // fprintf(stderr, "counted %d times and grounded %d; ungrd: %d\n", counts, total_grounded, ungrounded_voxels);
    return ungrounded_voxels > 0;
}

GArray* insert_flips(multi_bot_commands_t* mbc, matrix_t* mdl) {
    equalize_multi_bot_commands(*mbc);
    GArray* cmds = merge_bot_commands(*mbc);

    matrix_t m = make_matrix(mdl->resolution);
    state_t state = make_start_state_from_matrix(m, Thunderbolt);
    trace_t trace;
    command_t* commands = (command_t*)cmds->data;

    int time = 0;
    while (state.n_bots > 0) {
        time++;
        int n_bots = state.n_bots;
        execution_t exec = start_timestep(state, commands);

        for (int i = 0; i < state.n_bots; i++) {
            bot_t bot = state.bots[i];
            exec_bot(&exec, state.bots[i], commands[i]);
        }

        bool need_high = has_ungrounded_voxels(&m);
	if (need_high != (state.harmonics == High)) {
            *commands = flip_cmd();
            exec.new_state.harmonics = (state.harmonics + 1) % 2;
        }

        state = finish_timestep(&exec);
        commands += n_bots;
    }
    return cmds;
}


GArray* calc_multi_bot_default_trace(matrix_t *m, bot_t initial_bot) {

    region_t rf = make_region(create_coord(1,0,1), create_coord(m->resolution-2, m->resolution-1, m->resolution-2));

    int cols = 19;
    int rows = 2;
    bool reserve_bot0 = true;
    int n_regions = cols*rows;

    //multi_bot_commands_t mbc = make_multi_bot_commands(rows*cols);
    bot_commands_t bc = make_bot_commands(initial_bot);

    region_grid_t grid = split_region(rf, rows, cols);

    multi_bot_commands_t mbc = split_up_bots(&grid, bc, reserve_bot0);
    //add_cmd(mbc.bot_commands[0].cmds, flip_cmd());

    int first_bot = reserve_bot0?1:0;

    fill_one_region(m, &mbc.bot_commands[first_bot], grid.regions[0]);
    for (int i=1; i < n_regions; i++) {
        fill_one_region(m, &mbc.bot_commands[first_bot+i], grid.regions[n_regions-i]);
    }
    gather_grid_bots(&mbc, m->resolution-1, reserve_bot0?1:0);

    move_bot_in_multibot_setting(&mbc.bot_commands[first_bot], sub_coords(create_coord(0,get_bot_pos(&mbc.bot_commands[first_bot]).y,0), get_bot_pos(&mbc.bot_commands[first_bot])));

    if (reserve_bot0) {
    	move_bot_in_multibot_setting(&mbc.bot_commands[first_bot], sub_coords(create_coord(0,1,0), get_bot_pos(&mbc.bot_commands[first_bot])));
	equalize_some_multi_bot_commands(mbc, 2);
        fusion(&mbc.bot_commands[0], &mbc.bot_commands[first_bot]);
    } else {
    	move_bot_in_multibot_setting(&mbc.bot_commands[first_bot], sub_coords(create_coord(0,0,0), get_bot_pos(&mbc.bot_commands[first_bot])));
    }

    add_cmd(mbc.bot_commands[0].cmds, halt_cmd());

    if (reserve_bot0) {
        return insert_flips(&mbc, m);
 //       return merge_bot_commands(mbc);
    } else {
        return merge_bot_commands(mbc);
    }
}
