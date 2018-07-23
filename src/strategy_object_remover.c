#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmodule.h>
#include "default_trace.h"
#include "region.h"
#include "move_helper.h"
#include "multi_bot_helpers.h"

extern int
gather_bots(multi_bot_commands_t* mbc);

extern void
calc_multi_bot_constructor_trace(matrix_t *m, multi_bot_commands_t *mbc);


static void 
debug_bb(region_t *bb){
    printf("BB: min=(%d, %d, %d); max=(%d, %d, %d)\n", bb->c_min.x, bb->c_min.y, bb->c_min.z, bb->c_max.x, bb->c_max.y, bb->c_max.z);
}

static void
debug_coord(coord_t c){
    printf("Coord: %d, %d, %d\n", c.x, c.y, c.z);
}


void 
copy_region(region_t* target, region_t *src){
    memcpy(target, src, sizeof(region_t));
}




static void 
wait_n_rounds(bot_commands_t *bc, int n){
    assert(n>=0);
    for(int i=0;i<n;++i){
        add_cmd(bc->cmds, wait_cmd());
    }
}


int
move_bot_in_multibot_setting(bot_commands_t *bc, coord_t rel_movement){

    int steps =	goto_rel_pos(rel_movement, bc->cmds);
    set_bot_pos(bc, add_coords(get_bot_pos(bc), rel_movement));
    return steps;
}



int
move_bot_in_multibot_setting_delayed(bot_commands_t *bc, coord_t rel_movement){
    int steps = 1;
    add_cmd(bc->cmds, wait_cmd());
    steps += goto_rel_pos(rel_movement, bc->cmds);
    set_bot_pos(bc, add_coords(get_bot_pos(bc), rel_movement));
    return steps;
}









/* returns the number of full voxels inside boundary box */
int 
calc_boundary_box_in_region(matrix_t *mdl, region_t r, region_t* bb){

    int num_filled = 0;

    
    if(region_is_empty(mdl, r)){
        /* return original bb with filled size 0*/
        copy_region(bb, &r);
        return num_filled;
    }

    /* initialize max and min - normalize min and max of the region to accept all kind of regions
    //not needed region shall always be wellformed!
    coord_t minFull = create_coord(MAX(r.c_max.x, r.c_min.x), MAX(r.c_max.y, r.c_min.y), MAX(r.c_max.y, r.c_min.y));
    coord_t maxFull = create_coord(MIN(r.c_max.x, r.c_min.x), MIN(r.c_max.y, r.c_min.y), MIN(r.c_max.y, r.c_min.y));
    */

    coord_t minFull = r.c_max;
    coord_t maxFull = r.c_min;


    //debug_coord(minFull);
    //debug_coord(maxFull);

    /* iterate through voxels. update min and max, count number of full voxels */
	FOR_EACH_COORD(cur, r)
	{
		if (get_voxel(mdl, cur) == Full)
		{
            num_filled += 1;
			if (cur.x < minFull.x)
			{
				minFull.x = cur.x;
			}
			if (cur.y < minFull.y)
			{
				minFull.y = cur.y;
			}
			if (cur.z < minFull.z)
			{
				minFull.z = cur.z;
			}
			if (cur.x > maxFull.x)
			{
				maxFull.x = cur.x;
			}
			if (cur.y > maxFull.y)
			{
				maxFull.y = cur.y;
			}
			if (cur.z > maxFull.z)
			{
				maxFull.z = cur.z;
			}
		}
	}
	END_FOR_EACH_COORD;

    //debug_coord(minFull);
    //debug_coord(maxFull);

    bb->c_min = minFull;
    bb->c_max = maxFull;

    return num_filled;
}


/* assumes the 4 bots are sitting on the top of the bb 
   b1 over c_min. b2, b3, b4 oriented clockwise
*/

/* visualization of the state

y
^
|     
|     b           b <- bots
|     +-----------+ <- upper end of bb
|     |x          |
|     |x (zeug)  x|
|     |xxxxxxxxxxx|
|     |x          |
|     +-----------+ <- lower end of bb
|
|
+----------------------------------------> x(,z)

z
^
|                  bb max
|       +-----------+
|       |b2       b3|
|       |           |  orientation of bots is
|       |           |  clockwise starting with
|       |           |  b1 over c_min coord of
|       |b1       b4|  the boundary box
|       +-----------+
|      bb min
|
+------------------------------------------> x


*/



int
void_a_boundary_box(matrix_t *mdl, region_t* bb, multi_bot_commands_t *mbc){

    int time=0;

    xyz_t cur_bot_y = bb->c_max.y+1;

	for (xyz_t y = cur_bot_y; y > bb->c_min.y; y--)
	{
    	region_t next_row = make_region(create_coord(bb->c_min.x, y-1, bb->c_min.z), create_coord(bb->c_max.x, y-1, bb->c_max.z));
		if (region_is_empty(mdl, next_row)) continue;

        /* move bots down */
        time += move_bot_in_multibot_setting(&mbc->bot_commands[0], create_coord(0, y-cur_bot_y, 0));
        move_bot_in_multibot_setting(&mbc->bot_commands[1], create_coord(0, y-cur_bot_y, 0));
        move_bot_in_multibot_setting(&mbc->bot_commands[2], create_coord(0, y-cur_bot_y, 0));
        move_bot_in_multibot_setting(&mbc->bot_commands[3], create_coord(0, y-cur_bot_y, 0));

        cur_bot_y = y;

        /* void the region below */
		add_cmd(mbc->bot_commands[0].cmds, gvoid_cmd(create_coord(0, -1, 0), sub_coords(get_bot_pos(&mbc->bot_commands[2]), get_bot_pos(&mbc->bot_commands[0]))));
		add_cmd(mbc->bot_commands[1].cmds, gvoid_cmd(create_coord(0, -1, 0), sub_coords(get_bot_pos(&mbc->bot_commands[3]), get_bot_pos(&mbc->bot_commands[1]))));
		add_cmd(mbc->bot_commands[2].cmds, gvoid_cmd(create_coord(0, -1, 0), sub_coords(get_bot_pos(&mbc->bot_commands[0]), get_bot_pos(&mbc->bot_commands[2]))));
		add_cmd(mbc->bot_commands[3].cmds, gvoid_cmd(create_coord(0, -1, 0), sub_coords(get_bot_pos(&mbc->bot_commands[1]), get_bot_pos(&mbc->bot_commands[3]))));

        time += 1;
	}
    return time;
}

int
move_up_and_to_next_bb_xdirection(matrix_t *mdl, region_t* bb, multi_bot_commands_t *mbc){
    
    xyz_t cur_bot_y = get_bot_pos(&mbc->bot_commands[0]).y;
    xyz_t next_bot_y = bb->c_max.y+1;


    /* move up again */
    int time_up = move_bot_in_multibot_setting(&mbc->bot_commands[0], create_coord(0, next_bot_y-cur_bot_y, 0));
    move_bot_in_multibot_setting(&mbc->bot_commands[1], create_coord(0, next_bot_y-cur_bot_y, 0));
    move_bot_in_multibot_setting(&mbc->bot_commands[2], create_coord(0, next_bot_y-cur_bot_y, 0));
    move_bot_in_multibot_setting(&mbc->bot_commands[3], create_coord(0, next_bot_y-cur_bot_y, 0));


    /* move along X axis (front is in positive direction) */
    //for bots [1, 2]
    xyz_t xdiff_front = bb->c_max.x - mbc->bot_commands[1].bot.pos.x;
    coord_t xmove_front = create_coord(xdiff_front, 0, 0);
    //for bots [0, 3]
    xyz_t xdiff_back = bb->c_min.x - mbc->bot_commands[0].bot.pos.x;
    coord_t xmove_back = create_coord(xdiff_back, 0, 0);

    int time_front = 0;
    int time_back = 0;
    if((abs(xdiff_front) >= abs(mbc->bot_commands[1].bot.pos.x - mbc->bot_commands[0].bot.pos.x)) && (abs(mbc->bot_commands[1].bot.pos.x - mbc->bot_commands[0].bot.pos.x) >= 15)){
        //we are close together and need to go far. postpone the backward bots by one round to avoid collision            
        if(xdiff_front > 0){
            //moving forwards
            time_back = move_bot_in_multibot_setting_delayed(&mbc->bot_commands[0], xmove_back);
            time_front = move_bot_in_multibot_setting(&mbc->bot_commands[1], xmove_front);
            move_bot_in_multibot_setting(&mbc->bot_commands[2], xmove_front);
            move_bot_in_multibot_setting_delayed(&mbc->bot_commands[3], xmove_back);
        } else {
            //moving backwards
            time_back = move_bot_in_multibot_setting(&mbc->bot_commands[0], xmove_back);
            time_front = move_bot_in_multibot_setting_delayed(&mbc->bot_commands[1], xmove_front);
            move_bot_in_multibot_setting_delayed(&mbc->bot_commands[2], xmove_front);
            move_bot_in_multibot_setting(&mbc->bot_commands[3], xmove_back);
        }
    }

    int time_move=MAX(time_back, time_front);
    if(time_move > time_back){
        wait_n_rounds(&mbc->bot_commands[0], time_move - time_back);
        wait_n_rounds(&mbc->bot_commands[3], time_move - time_back);
    }
    if(time_move > time_front){
        wait_n_rounds(&mbc->bot_commands[1], time_move - time_front);
        wait_n_rounds(&mbc->bot_commands[2], time_move - time_front);
    }
    return time_up + time_move;
    
}


int
move_up_and_to_next_bb_zdirection(matrix_t *mdl, region_t* bb, multi_bot_commands_t *mbc){
    

    xyz_t cur_bot_y = get_bot_pos(&mbc->bot_commands[0]).y;
    xyz_t next_bot_y = bb->c_max.y+1;

    /* move up again */
    int time_up = move_bot_in_multibot_setting(&mbc->bot_commands[0], create_coord(0, next_bot_y-cur_bot_y, 0));
    move_bot_in_multibot_setting(&mbc->bot_commands[1], create_coord(0, next_bot_y-cur_bot_y, 0));
    move_bot_in_multibot_setting(&mbc->bot_commands[2], create_coord(0, next_bot_y-cur_bot_y, 0));
    move_bot_in_multibot_setting(&mbc->bot_commands[3], create_coord(0, next_bot_y-cur_bot_y, 0));

    /* move along Z axis (front is in positive direction) */
    //for bots [2, 3]
    xyz_t zdiff_front = bb->c_max.z - mbc->bot_commands[2].bot.pos.z;
    coord_t zmove_front = create_coord(0,0, zdiff_front);
    //for bots [0, 1]
    xyz_t zdiff_back = bb->c_min.z - mbc->bot_commands[0].bot.pos.z;
    coord_t zmove_back = create_coord(0,0, zdiff_back);

    int time_front = 0;
    int time_back = 0;
    if((abs(zdiff_front) >= abs(mbc->bot_commands[2].bot.pos.z - mbc->bot_commands[0].bot.pos.z)) && (abs(mbc->bot_commands[2].bot.pos.z - mbc->bot_commands[0].bot.pos.z) >= 15)){
        //we are close together and need to go far. postpone the backward bots by one round to avoid collision            
        if(zdiff_front > 0){
            //moving forwards
            time_front = move_bot_in_multibot_setting(&mbc->bot_commands[2], zmove_front);
            move_bot_in_multibot_setting(&mbc->bot_commands[3], zmove_front);
            time_back = move_bot_in_multibot_setting_delayed(&mbc->bot_commands[0], zmove_back);
            move_bot_in_multibot_setting_delayed(&mbc->bot_commands[1], zmove_back);
        } else {
            //moving backwards
            time_front = move_bot_in_multibot_setting_delayed(&mbc->bot_commands[2], zmove_front);
            move_bot_in_multibot_setting_delayed(&mbc->bot_commands[3], zmove_front);
            time_back = move_bot_in_multibot_setting(&mbc->bot_commands[0], zmove_back);
            move_bot_in_multibot_setting(&mbc->bot_commands[1], zmove_back);
        }
    }

    int time_move=MAX(time_back, time_front);
    if(time_move > time_back){
        wait_n_rounds(&mbc->bot_commands[0], time_move - time_back);
        wait_n_rounds(&mbc->bot_commands[1], time_move - time_back);
    }
    if(time_move > time_front){
        wait_n_rounds(&mbc->bot_commands[2], time_move - time_front);
        wait_n_rounds(&mbc->bot_commands[3], time_move - time_front);
    }
    return time_up + time_move;

}


void
bot_spawn(matrix_t *mdl, multi_bot_commands_t *mbc, int mbc_id_src, int mbc_id_trg, coord_t spawn_vect){
    /* spawn the new bot */
    mbc->bot_commands[mbc_id_trg] = fission(&mbc->bot_commands[mbc_id_src], spawn_vect);
}


void
bot_spawn_quadrupel(matrix_t *mdl, multi_bot_commands_t *mbc, int mbc_id_src, int length_side){

    
    //spawn bot1
    bot_spawn(mdl, mbc, mbc_id_src, mbc_id_src + 1, create_coord(1,0,0));
    int time = move_bot_in_multibot_setting(&mbc->bot_commands[mbc_id_src + 1], create_coord(length_side-2, 0, 0));
    wait_n_rounds(&mbc->bot_commands[mbc_id_src], time);
    //spawn bot 2
    bot_spawn(mdl, mbc, mbc_id_src + 1, mbc_id_src + 2, create_coord(0,0,1));
    //spawn bot 3
    bot_spawn(mdl, mbc, mbc_id_src,     mbc_id_src + 3, create_coord(0,0,1));

    time = move_bot_in_multibot_setting(&mbc->bot_commands[mbc_id_src+2], create_coord(0, 0, length_side-2));
    move_bot_in_multibot_setting(&mbc->bot_commands[mbc_id_src+3], create_coord(0, 0, length_side-2));
    wait_n_rounds(&mbc->bot_commands[mbc_id_src],   time);
    wait_n_rounds(&mbc->bot_commands[mbc_id_src+1], time);

}

void
bot_spawn_rect(matrix_t *mdl, multi_bot_commands_t *mbc, int mbc_id_src, int length_x, int length_z){

    
    //spawn bot1
    bot_spawn(mdl, mbc, mbc_id_src, mbc_id_src + 1, create_coord(1,0,0));
    int time = move_bot_in_multibot_setting(&mbc->bot_commands[mbc_id_src + 1], create_coord(length_x-2, 0, 0));
    wait_n_rounds(&mbc->bot_commands[mbc_id_src], time);
    //spawn bot 2
    bot_spawn(mdl, mbc, mbc_id_src + 1, mbc_id_src + 2, create_coord(0,0,1));
    //spawn bot 3
    bot_spawn(mdl, mbc, mbc_id_src,     mbc_id_src + 3, create_coord(0,0,1));

    time = move_bot_in_multibot_setting(&mbc->bot_commands[mbc_id_src+2], create_coord(0, 0, length_z-2));
    move_bot_in_multibot_setting(&mbc->bot_commands[mbc_id_src+3], create_coord(0, 0, length_z-2));
    wait_n_rounds(&mbc->bot_commands[mbc_id_src],   time);
    wait_n_rounds(&mbc->bot_commands[mbc_id_src+1], time);

}






#define QLENGTH 30

GArray* 
exec_test_bb_flush(matrix_t *mdl, bot_t *bot1){
    int quadruple_count = 1;
    multi_bot_commands_t mbc = make_multi_bot_commands(4 * quadruple_count);

    region_t bb_overall = make_region(create_coord(0,0,0), create_coord(5,5,5));

    mbc.bot_commands[0] = make_bot_commands(*bot1);


    int filled_voxels = calc_boundary_box_in_region(mdl, make_region(create_coord(0,0,0), create_coord(mdl->resolution-1, mdl->resolution-1, mdl->resolution-1)), &bb_overall);


    /* move initial bot above min of bounding box */
    coord_t above_min_bb = create_coord(bb_overall.c_min.x, bb_overall.c_max.y +1, bb_overall.c_min.z);
    move_bot_in_multibot_setting(&mbc.bot_commands[0], sub_coords(above_min_bb, mbc.bot_commands[0].bot.pos) );

	// turn off gravity
    add_cmd(mbc.bot_commands[0].cmds, flip_cmd());


    int cols = (bb_overall.c_max.x - bb_overall.c_min.x) / QLENGTH;
    if((cols * QLENGTH) < (bb_overall.c_max.x - bb_overall.c_min.x)) {
        cols += 1;
    }
    int rows = (bb_overall.c_max.z - bb_overall.c_min.z) / QLENGTH;
    if((rows * QLENGTH) < (bb_overall.c_max.z - bb_overall.c_min.z)) {
        rows += 1;
    }


    
    region_grid_t rgrid;
    rgrid = split_region(bb_overall, rows, cols);
    region_t r0 = get_grid_region(rgrid, 0,0);


    bot_spawn_rect(mdl, &mbc, 0, region_get_xsize(r0), region_get_zsize(r0));
    region_t r;
    for(int z=0; z<rows; ++z){
        r = get_grid_region(rgrid, z, 0);
        move_up_and_to_next_bb_zdirection(mdl, &r, &mbc );
        for(int x=0; x<cols; ++x){
            r = get_grid_region(rgrid, z, x);
            move_up_and_to_next_bb_xdirection(mdl, &r, &mbc );
            void_a_boundary_box(mdl, &r, &mbc);
        }
    }


    mbc.n_bots = 4;
    return merge_bot_commands(mbc);

}


GArray* 
exec_test_bb_flush_and_rebuild(matrix_t *mdl, matrix_t *targetmdl, bot_t *bot1){
    int quadruple_count = 1;
    multi_bot_commands_t mbc = make_multi_bot_commands(4 * quadruple_count);

    region_t bb_overall = make_region(create_coord(0,0,0), create_coord(5,5,5));

    mbc.bot_commands[0] = make_bot_commands(*bot1);


    int filled_voxels = calc_boundary_box_in_region(mdl, make_region(create_coord(0,0,0), create_coord(mdl->resolution-1, mdl->resolution-1, mdl->resolution-1)), &bb_overall);


    /* move initial bot above min of bounding box */
    coord_t above_min_bb = create_coord(bb_overall.c_min.x, bb_overall.c_max.y +1, bb_overall.c_min.z);
    move_bot_in_multibot_setting(&mbc.bot_commands[0], sub_coords(above_min_bb, mbc.bot_commands[0].bot.pos) );

	// turn off gravity
    add_cmd(mbc.bot_commands[0].cmds, flip_cmd());


    int cols = (bb_overall.c_max.x - bb_overall.c_min.x) / QLENGTH;
    if((cols * QLENGTH) < (bb_overall.c_max.x - bb_overall.c_min.x)) {
        cols += 1;
    }
    int rows = (bb_overall.c_max.z - bb_overall.c_min.z) / QLENGTH;
    if((rows * QLENGTH) < (bb_overall.c_max.z - bb_overall.c_min.z)) {
        rows += 1;
    }


    
    region_grid_t rgrid;
    rgrid = split_region(bb_overall, rows, cols);
    region_t r0 = get_grid_region(rgrid, 0,0);


    bot_spawn_rect(mdl, &mbc, 0, region_get_xsize(r0), region_get_zsize(r0));
    region_t r;
    for(int z=0; z<rows; ++z){
        r = get_grid_region(rgrid, z, 0);
        move_up_and_to_next_bb_zdirection(mdl, &r, &mbc );
        for(int x=0; x<cols; ++x){
            r = get_grid_region(rgrid, z, x);
            move_up_and_to_next_bb_xdirection(mdl, &r, &mbc );
            void_a_boundary_box(mdl, &r, &mbc);
        }
    }

    mbc.n_bots = 4;

    calc_multi_bot_constructor_trace(targetmdl, &mbc);




    
    return merge_bot_commands(mbc);

}




GArray* 
exec_test_bb_flush_and_exit(matrix_t *mdl, bot_t *bot1){
    int quadruple_count = 1;
    multi_bot_commands_t mbc = make_multi_bot_commands(4 * quadruple_count);

    region_t bb_overall = make_region(create_coord(0,0,0), create_coord(5,5,5));

    mbc.bot_commands[0] = make_bot_commands(*bot1);


    int filled_voxels = calc_boundary_box_in_region(mdl, make_region(create_coord(0,0,0), create_coord(mdl->resolution-1, mdl->resolution-1, mdl->resolution-1)), &bb_overall);


    /* move initial bot above min of bounding box */
    coord_t above_min_bb = create_coord(bb_overall.c_min.x, bb_overall.c_max.y +1, bb_overall.c_min.z);
    move_bot_in_multibot_setting(&mbc.bot_commands[0], sub_coords(above_min_bb, mbc.bot_commands[0].bot.pos) );

	// turn off gravity
    add_cmd(mbc.bot_commands[0].cmds, flip_cmd());


    int cols = (bb_overall.c_max.x - bb_overall.c_min.x) / QLENGTH;
    if((cols * QLENGTH) < (bb_overall.c_max.x - bb_overall.c_min.x)) {
        cols += 1;
    }
    int rows = (bb_overall.c_max.z - bb_overall.c_min.z) / QLENGTH;
    if((rows * QLENGTH) < (bb_overall.c_max.z - bb_overall.c_min.z)) {
        rows += 1;
    }


    
    region_grid_t rgrid;
    rgrid = split_region(bb_overall, rows, cols);
    region_t r0 = get_grid_region(rgrid, 0,0);


    bot_spawn_rect(mdl, &mbc, 0, region_get_xsize(r0), region_get_zsize(r0));
    region_t r;
    for(int z=0; z<rows; ++z){
        r = get_grid_region(rgrid, z, 0);
        move_up_and_to_next_bb_zdirection(mdl, &r, &mbc );
        for(int x=0; x<cols; ++x){
            r = get_grid_region(rgrid, z, x);
            move_up_and_to_next_bb_xdirection(mdl, &r, &mbc );
            void_a_boundary_box(mdl, &r, &mbc);
        }
    }
    gather_bots(&mbc);

    move_bot_in_multibot_setting(&mbc.bot_commands[0], sub_coords(create_coord(0,get_bot_pos(&mbc.bot_commands[0]).y,0), get_bot_pos(&mbc.bot_commands[0])));
    move_bot_in_multibot_setting(&mbc.bot_commands[0], sub_coords(create_coord(0,0,0), get_bot_pos(&mbc.bot_commands[0])));

    add_cmd(mbc.bot_commands[0].cmds, flip_cmd());
    add_cmd(mbc.bot_commands[0].cmds, halt_cmd());


    mbc.n_bots = 4;
    return merge_bot_commands(mbc);

}





