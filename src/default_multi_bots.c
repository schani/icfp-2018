#include "default_trace.h"

#include "model.h"
#include "move_helper.h"
#include "multi_bot_helpers.h"
#include "strategy_basic.h"

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
split_up_bots(region_grid_t* r, bot_commands_t initial_bot) {
    multi_bot_commands_t mbc = make_multi_bot_commands(r->rows*r->cols);
    mbc.bot_commands[0] = initial_bot;
    int n_bots = mbc.n_bots;

    mbc.n_bots = 1;
    move_bot_in_multibot_setting(&mbc.bot_commands[0], r->regions[0].c_min);
    for (int i=1; i<n_bots; i++) {
        mbc.bot_commands[i] = fission_with_m(&mbc.bot_commands[0], create_coord(0, 1, 0), 0);
        mbc.n_bots++;
        move_bot_in_multibot_setting(&mbc.bot_commands[i], above(r->regions[n_bots-i].c_min));
        equalize_multi_bot_commands(mbc);    
    }
    equalize_multi_bot_commands(mbc);
    return mbc;
}

int
gather_bots(multi_bot_commands_t* mbc) {
    move_bot_in_multibot_setting(&mbc->bot_commands[0], sub_coords(create_coord(0,get_bot_pos(&mbc->bot_commands[0]).y,0), get_bot_pos(&mbc->bot_commands[0])));
    move_bot_in_multibot_setting(&mbc->bot_commands[0], create_coord(0, -1, 0));
    equalize_multi_bot_commands(*mbc);

    coord_t merging_coords = above(get_bot_pos(&mbc->bot_commands[0]));

    for (int b = mbc->n_bots - 1; b > 0; b--) {
        assert(b!=0);
        move_bot_in_multibot_setting(&mbc->bot_commands[b], sub_coords(merging_coords, get_bot_pos(&mbc->bot_commands[b])));
        equalize_multi_bot_commands(*mbc);
        fusion(&mbc->bot_commands[0], &mbc->bot_commands[b]);
        equalize_multi_bot_commands(*mbc);
    }
}


void
move_bots_to_grid(region_grid_t *r, multi_bot_commands_t *mbc){
    move_bot_in_multibot_setting(&mbc->bot_commands[0], r->regions[0].c_min);
    for (int i=1; i<4; i++) {
      //mbc.bot_commands[i] = fission_with_m(&mbc.bot_commands[0], create_coord(0, 1, 0), 0);
      //mbc.n_bots++;
        move_bot_in_multibot_setting(&mbc->bot_commands[i], above(r->regions[4-i].c_min));
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
    
    move_bots_to_grid(&grid, mbc);
 

    fill_one_region(m, &mbc->bot_commands[0], get_grid_region(grid, 0, 0));
    for (int i=1; i < mbc->n_bots; i++) {
        fill_one_region(m, &mbc->bot_commands[i], grid.regions[mbc->n_bots-i]);
    }

    equalize_multi_bot_commands(*mbc);

    gather_bots(mbc);

    move_bot_in_multibot_setting(&mbc->bot_commands[0], sub_coords(create_coord(0,get_bot_pos(&mbc->bot_commands[0]).y,0), get_bot_pos(&mbc->bot_commands[0])));
    move_bot_in_multibot_setting(&mbc->bot_commands[0], sub_coords(create_coord(0,0,0), get_bot_pos(&mbc->bot_commands[0])));

    add_cmd(mbc->bot_commands[0].cmds, flip_cmd());
    add_cmd(mbc->bot_commands[0].cmds, halt_cmd());

    
}



GArray* calc_multi_bot_default_trace(matrix_t *m, bot_t initial_bot) {
    region_t rf = matrix_region(m);

    int cols = 2;
    int rows = 2;

    //multi_bot_commands_t mbc = make_multi_bot_commands(rows*cols);
    bot_commands_t bc = make_bot_commands(initial_bot);
    add_cmd(bc.cmds, flip_cmd());

    // FIXME implement with gravity

    region_grid_t grid = split_region(rf, rows, cols);

    multi_bot_commands_t mbc = split_up_bots(&grid, bc);

    // FIXME generic fission
//    mbc.bot_commands[1] = fission(&mbc.bot_commands[0], create_coord(1, 0, 0));
//    mbc.bot_commands[2] = fission(&mbc.bot_commands[0], create_coord(0, 0, 1));
//    mbc.bot_commands[3] = fission(&mbc.bot_commands[0], create_coord(1, 0, 0));

//    move_bot_in_multibot_setting(&mbc.bot_commands[0], sub_coords(above(get_grid_region(grid, 0, 0).c_min), get_bot_pos(&mbc.bot_commands[0])));
 //   move_bot_in_multibot_setting(&mbc.bot_commands[1], sub_coords(above(get_grid_region(grid, 1, 1).c_min), get_bot_pos(&mbc.bot_commands[1])));
 //   move_bot_in_multibot_setting(&mbc.bot_commands[2], sub_coords(above(get_grid_region(grid, 1, 0).c_min), get_bot_pos(&mbc.bot_commands[2])));
 //   move_bot_in_multibot_setting(&mbc.bot_commands[3], sub_coords(above(get_grid_region(grid, 0, 1).c_min), get_bot_pos(&mbc.bot_commands[3])));
 //   equalize_multi_bot_commands(mbc);
    // end fission

    fill_one_region(m, &mbc.bot_commands[0], get_grid_region(grid, 0, 0));
    for (int i=1; i < mbc.n_bots; i++) {
        fill_one_region(m, &mbc.bot_commands[i], grid.regions[mbc.n_bots-i]);
    }
    //fill_one_region(m, &mbc.bot_commands[0], get_grid_region(grid, 0, 0));
    //fill_one_region(m, &mbc.bot_commands[1], get_grid_region(grid, 1, 1));
    //fill_one_region(m, &mbc.bot_commands[2], get_grid_region(grid, 1, 0));
    //fill_one_region(m, &mbc.bot_commands[3], get_grid_region(grid, 0, 1));

    equalize_multi_bot_commands(mbc);

    // FIXME generic fusion
    //move_bot_in_multibot_setting(&mbc.bot_commands[2], add_coords(sub_coords(get_bot_pos(&mbc.bot_commands[0]), get_bot_pos(&mbc.bot_commands[2])), create_coord(0,0,1)));
    //move_bot_in_multibot_setting(&mbc.bot_commands[3], add_coords(sub_coords(get_bot_pos(&mbc.bot_commands[0]), get_bot_pos(&mbc.bot_commands[3])), create_coord(1,0,0)));
    //equalize_multi_bot_commands(mbc);
    //fusion(&mbc.bot_commands[0], &mbc.bot_commands[3]);
    //move_bot_in_multibot_setting(&mbc.bot_commands[1], add_coords(sub_coords(get_bot_pos(&mbc.bot_commands[0]), get_bot_pos(&mbc.bot_commands[1])), create_coord(1,0,0)));
    //equalize_multi_bot_commands(mbc);
    //fusion(&mbc.bot_commands[0], &mbc.bot_commands[2]);
    //equalize_multi_bot_commands(mbc);
    //fusion(&mbc.bot_commands[0], &mbc.bot_commands[1]);
    // end fusion
    gather_bots(&mbc);

    move_bot_in_multibot_setting(&mbc.bot_commands[0], sub_coords(create_coord(0,get_bot_pos(&mbc.bot_commands[0]).y,0), get_bot_pos(&mbc.bot_commands[0])));
    move_bot_in_multibot_setting(&mbc.bot_commands[0], sub_coords(create_coord(0,0,0), get_bot_pos(&mbc.bot_commands[0])));

    add_cmd(mbc.bot_commands[0].cmds, flip_cmd());
    add_cmd(mbc.bot_commands[0].cmds, halt_cmd());

    mbc.n_bots = 4;
    return merge_bot_commands(mbc);
}
