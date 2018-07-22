#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmodule.h>
#include "default_trace.h"

#include "move_helper.h"
#include "multi_bot_helpers.h"


void 
copy_region(region_t* target, region_t *src){
    memcpy(target, src, sizeof(region_t));
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

    return num_filled;
}

typedef struct{
    bot_commands_t *b1;
    bot_commands_t *b2;
} bc2_t;


typedef struct{
    bot_commands_t *b1;
    bot_commands_t *b2;
    bot_commands_t *b3;
    bot_commands_t *b4;
} bc4_t;

typedef struct{
    bot_commands_t *b1;
    bot_commands_t *b2;
    bot_commands_t *b3;
    bot_commands_t *b4;
    bot_commands_t *b5;
    bot_commands_t *b6;
    bot_commands_t *b7;
    bot_commands_t *b8;
} bc8_t;

typedef struct{
    GArray * b1_cmds;
    GArray * b2_cmds;
    GArray * b3_cmds;
    GArray * b4_cmds;
} cmd4;

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



cmd4
void_a_boundary_box(matrix_t *mdl, region_t* bb){

    cmd4 ret_val;

	GArray *b1_cmds = g_array_new(FALSE, FALSE, sizeof(command_t));
	GArray *b2_cmds = g_array_new(FALSE, FALSE, sizeof(command_t));
	GArray *b3_cmds = g_array_new(FALSE, FALSE, sizeof(command_t));
	GArray *b4_cmds = g_array_new(FALSE, FALSE, sizeof(command_t));

    ret_val.b1_cmds = b1_cmds;
    ret_val.b2_cmds = b2_cmds;
    ret_val.b3_cmds = b3_cmds;
    ret_val.b4_cmds = b4_cmds;

    xyz_t cur_bot_y = bb->c_max.y+1;

	for (xyz_t y = bb->c_max.y+1; y > bb->c_min.y; y++)
	{
    	region_t next_row = make_region(create_coord(bb->c_min.x, y-1, bb->c_min.z), create_coord(bb->c_max.x, y-1, bb->c_max.z));
		if (region_is_empty(mdl, next_row)) continue;

        /* move bot down */
		goto_rel_pos(create_coord(0, y-cur_bot_y, 0), b1_cmds);
		goto_rel_pos(create_coord(0, y-cur_bot_y, 0), b2_cmds);
		goto_rel_pos(create_coord(0, y-cur_bot_y, 0), b3_cmds);
		goto_rel_pos(create_coord(0, y-cur_bot_y, 0), b4_cmds);
        cur_bot_y = y;

        /* void the region below */
		add_cmd(b1_cmds, gvoid_cmd(create_coord(0, -1, 0), create_coord(bb->c_max.x, 0, bb->c_max.z)));
		add_cmd(b2_cmds, gvoid_cmd(create_coord(0, -1, 0), create_coord(bb->c_max.x, 0, bb->c_min.z)));
		add_cmd(b3_cmds, gvoid_cmd(create_coord(0, -1, 0), create_coord(bb->c_min.x, 0, bb->c_min.z)));
		add_cmd(b2_cmds, gvoid_cmd(create_coord(0, -1, 0), create_coord(bb->c_min.x, 0, bb->c_max.z)));
        
	}
    return ret_val;

}











void wait_n_rounds(GArray *cmds, int n){
    assert(n>=0);
    for(int i=0;i<n;++i){
        add_cmd(cmds, wait_cmd());
    }
}


/*
void spawn_at_pos(GArray *cmds, coord_t spawn_vect, int seed_m){
    add_cmd(cmds, fission_cmd(spawn_vect, seed_m));
}

*/



bc2_t
initial_spawn(matrix_t *mdl, bot_commands_t *b1, coord_t spawn_vect){
    bc2_t ret_val;
    ret_val.b1 = b1;

    /* spawn the new bot */
    *ret_val.b2 = fission(b1, spawn_vect);

    return ret_val;
}

bc4_t
move_b2_to_pos_and_duplicate(matrix_t *mdl, bc2_t bot_commands, coord_t b2_target, coord_t spawn_vect){
    bc4_t ret_val;

    ret_val.b1 = bot_commands.b1;
    ret_val.b2 = bot_commands.b2;


    /* move bot 2 to correct position and keep bot 1 waiting */
    int wait_time = goto_rel_pos(sub_coords(b2_target, get_bot_pos(bot_commands.b2)), bot_commands.b2->cmds);
    set_bot_pos(bot_commands.b2, b2_target);
    wait_n_rounds(bot_commands.b1->cmds, wait_time);

    /* spawn the new bots */
    *ret_val.b3 = fission(bot_commands.b1, spawn_vect);
    *ret_val.b4 = fission(bot_commands.b2, spawn_vect);

    return ret_val;
}

bc8_t
move_b3b4_to_pos_and_duplicate(matrix_t *mdl, bc4_t bot_commands, coord_t b3_target, coord_t b4_target, coord_t spawn_vect){
    bc8_t ret_val;

    ret_val.b1 = bot_commands.b1;
    ret_val.b2 = bot_commands.b2;
    ret_val.b3 = bot_commands.b3;
    ret_val.b4 = bot_commands.b4;

    int wait_time;
    /* move bot 3 and bot 4 to correct position and keep bot 1 and bot 2 waiting */
    int wait_time3 = goto_rel_pos(sub_coords(b3_target, get_bot_pos(bot_commands.b3)), bot_commands.b3->cmds);
    set_bot_pos(bot_commands.b3, b3_target);
    int wait_time4 = goto_rel_pos(sub_coords(b4_target, get_bot_pos(bot_commands.b4)), bot_commands.b4->cmds);
    set_bot_pos(bot_commands.b4, b4_target);
    
    wait_time = MAX(wait_time3, wait_time4);

    if(wait_time3 < wait_time){
        wait_n_rounds(ret_val.b3->cmds, wait_time-wait_time3);
    }
    if(wait_time4 < wait_time){
        wait_n_rounds(ret_val.b4->cmds, wait_time-wait_time4);
    }

    wait_n_rounds(bot_commands.b1->cmds, wait_time);
    wait_n_rounds(bot_commands.b2->cmds, wait_time);

    /* spawn the new bots */
    *ret_val.b5 = fission(bot_commands.b1, spawn_vect);
    *ret_val.b6 = fission(bot_commands.b2, spawn_vect);
    *ret_val.b7 = fission(bot_commands.b3, spawn_vect);
    *ret_val.b8 = fission(bot_commands.b4, spawn_vect);

    return ret_val;

}

void
move_b5b6b7b8_to_pos_and_void(matrix_t *mdl, bc8_t bot_commands, coord_t b5_target, coord_t b6_target, coord_t b7_target, coord_t b8_target){
    int wait_time;
    int wait_time_a;
    int wait_time_b;

    /* move bot 5, bot 6, bot 7, and bot 8 to correct position and keep others waiting */
    int wait_time5 = goto_rel_pos(sub_coords(b5_target, get_bot_pos(bot_commands.b5)), bot_commands.b5->cmds);
    set_bot_pos(bot_commands.b5, b5_target);
    int wait_time6 = goto_rel_pos(sub_coords(b6_target, get_bot_pos(bot_commands.b6)), bot_commands.b6->cmds);
    set_bot_pos(bot_commands.b6, b6_target);
    int wait_time7 = goto_rel_pos(sub_coords(b7_target, get_bot_pos(bot_commands.b7)), bot_commands.b7->cmds);
    set_bot_pos(bot_commands.b7, b7_target);
    int wait_time8 = goto_rel_pos(sub_coords(b8_target, get_bot_pos(bot_commands.b8)), bot_commands.b8->cmds);
    set_bot_pos(bot_commands.b8, b8_target);

    wait_time_a = MAX(wait_time5, wait_time6);
    wait_time_b = MAX(wait_time7, wait_time8);
    wait_time = MAX(wait_time_a, wait_time_b);
    
    if(wait_time5 < wait_time){
        wait_n_rounds(bot_commands.b5->cmds, wait_time-wait_time5);
    }
    if(wait_time6 < wait_time){
        wait_n_rounds(bot_commands.b6->cmds, wait_time-wait_time6);
    }
    if(wait_time7 < wait_time){
        wait_n_rounds(bot_commands.b7->cmds, wait_time-wait_time7);
    }
    if(wait_time8 < wait_time){
        wait_n_rounds(bot_commands.b8->cmds, wait_time-wait_time8);
    }

    wait_n_rounds(bot_commands.b1->cmds, wait_time);
    wait_n_rounds(bot_commands.b2->cmds, wait_time);
    wait_n_rounds(bot_commands.b3->cmds, wait_time);
    wait_n_rounds(bot_commands.b4->cmds, wait_time);

    /* void the region surrounded */
    add_cmd(bot_commands.b1->cmds, gvoid_cmd(create_coord( 1, 0,  1), create_coord( mdl->resolution-3,  mdl->resolution-1,  mdl->resolution-3)));
    add_cmd(bot_commands.b8->cmds, gvoid_cmd(create_coord(-1, 0, -1), create_coord(-mdl->resolution-3, -mdl->resolution-1, -mdl->resolution-3)));

    add_cmd(bot_commands.b2->cmds, gvoid_cmd(create_coord(-1, 0,  1), create_coord(-mdl->resolution-3,  mdl->resolution-1,  mdl->resolution-3)));
    add_cmd(bot_commands.b7->cmds, gvoid_cmd(create_coord( 1, 0, -1), create_coord( mdl->resolution-3, -mdl->resolution-1, -mdl->resolution-3)));

    add_cmd(bot_commands.b3->cmds, gvoid_cmd(create_coord( 1, 0, -1), create_coord( mdl->resolution-3,  mdl->resolution-1, -mdl->resolution-3)));
    add_cmd(bot_commands.b6->cmds, gvoid_cmd(create_coord(-1, 0,  1), create_coord(-mdl->resolution-3, -mdl->resolution-1,  mdl->resolution-3)));

    add_cmd(bot_commands.b4->cmds, gvoid_cmd(create_coord(-1, 0, -1), create_coord(-mdl->resolution-3,  mdl->resolution-1, -mdl->resolution-3)));
    add_cmd(bot_commands.b5->cmds, gvoid_cmd(create_coord( 1, 0,  1), create_coord( mdl->resolution-3, -mdl->resolution-1,  mdl->resolution-3)));

}


void
move_b5b6b7b8_to_pos_and_fuse(matrix_t *mdl, bc8_t bot_commands, coord_t b5_target, coord_t b6_target, coord_t b7_target, coord_t b8_target){
    int wait_time;
    int wait_time_a;
    int wait_time_b;

    /* move bot 5, bot 6, bot 7, and bot 8 to correct position and keep others waiting */
    int wait_time5 = goto_rel_pos(sub_coords(b5_target, get_bot_pos(bot_commands.b5)), bot_commands.b5->cmds);
    set_bot_pos(bot_commands.b5, b5_target);
    int wait_time6 = goto_rel_pos(sub_coords(b6_target, get_bot_pos(bot_commands.b6)), bot_commands.b6->cmds);
    set_bot_pos(bot_commands.b6, b6_target);
    int wait_time7 = goto_rel_pos(sub_coords(b7_target, get_bot_pos(bot_commands.b7)), bot_commands.b7->cmds);
    set_bot_pos(bot_commands.b7, b7_target);
    int wait_time8 = goto_rel_pos(sub_coords(b8_target, get_bot_pos(bot_commands.b8)), bot_commands.b8->cmds);
    set_bot_pos(bot_commands.b8, b8_target);

    wait_time_a = MAX(wait_time5, wait_time6);
    wait_time_b = MAX(wait_time7, wait_time8);
    wait_time = MAX(wait_time_a, wait_time_b);
    
    if(wait_time5 < wait_time){
        wait_n_rounds(bot_commands.b5->cmds, wait_time-wait_time5);
    }
    if(wait_time6 < wait_time){
        wait_n_rounds(bot_commands.b6->cmds, wait_time-wait_time6);
    }
    if(wait_time7 < wait_time){
        wait_n_rounds(bot_commands.b7->cmds, wait_time-wait_time7);
    }
    if(wait_time8 < wait_time){
        wait_n_rounds(bot_commands.b8->cmds, wait_time-wait_time8);
    }

    wait_n_rounds(bot_commands.b1->cmds, wait_time);
    wait_n_rounds(bot_commands.b2->cmds, wait_time);
    wait_n_rounds(bot_commands.b3->cmds, wait_time);
    wait_n_rounds(bot_commands.b4->cmds, wait_time);

    /* fuse the bots */
    fusion(bot_commands.b1, bot_commands.b5);
    fusion(bot_commands.b2, bot_commands.b6);
    fusion(bot_commands.b3, bot_commands.b7);
    fusion(bot_commands.b4, bot_commands.b8);

}


void
move_b3b4_to_pos_and_fuse(matrix_t *mdl, bc4_t bot_commands, coord_t b3_target, coord_t b4_target, coord_t spawn_vect){

    int wait_time;
    /* move bot 3 and bot 4 to correct position and keep bot 1 and bot 2 waiting */
    int wait_time3 = goto_rel_pos(sub_coords(b3_target, get_bot_pos(bot_commands.b3)), bot_commands.b3->cmds);
    set_bot_pos(bot_commands.b3, b3_target);
    int wait_time4 = goto_rel_pos(sub_coords(b4_target, get_bot_pos(bot_commands.b4)), bot_commands.b4->cmds);
    set_bot_pos(bot_commands.b4, b4_target);
    
    wait_time = MAX(wait_time3, wait_time4);

    if(wait_time3 < wait_time){
        wait_n_rounds(bot_commands.b3->cmds, wait_time-wait_time3);
    }
    if(wait_time4 < wait_time){
        wait_n_rounds(bot_commands.b4->cmds, wait_time-wait_time4);
    }

    wait_n_rounds(bot_commands.b1->cmds, wait_time);
    wait_n_rounds(bot_commands.b2->cmds, wait_time);

    /* fuse the bots */
    fusion(bot_commands.b1, bot_commands.b3);
    fusion(bot_commands.b2, bot_commands.b4);

}


void
move_b2_to_pos_and_fuse(matrix_t *mdl, bc2_t bot_commands, coord_t b2_target, coord_t spawn_vect){
    

    /* move bot 2 to correct position and keep bot 1 waiting */
    int wait_time = goto_rel_pos(sub_coords(b2_target, get_bot_pos(bot_commands.b2)), bot_commands.b2->cmds);
    set_bot_pos(bot_commands.b2, b2_target);
    wait_n_rounds(bot_commands.b1->cmds, wait_time);

    /* fuse the bots */
    fusion(bot_commands.b1, bot_commands.b2);

}






GArray* 
exec_flush_at_once(matrix_t *mdl){
	GArray *cmds = g_array_new(FALSE, FALSE, sizeof(command_t));
    /* spawn new bot one x coord ahead */
    //spawn_at_pos(cmds, create_coord(1,0,0), 3);

    /* move new robot to end and spawn in z direction new robots */
    //cmd2 phase1_commands = move_b2_to_pos_and_duplicate_bots(mdl, create_coord(0,0,0), create_coord(1,0,0), create_coord(mdl->resolution-1,0,0),create_coord(0,0,1),2);








}




GArray* 
exec_some_trace(matrix_t *mdl)
{
	GArray *cmds = g_array_new(FALSE, FALSE, sizeof(command_t));

	// Calculate boundary box
	coord_t minFull, maxFull;
	calc_boundary_box(mdl, &minFull, &maxFull);

	coord_t curPos, startPos;
	int16_t x, y, z;

	// turn off gravity
	add_cmd(cmds, flip_cmd());

	startPos = add_coords(minFull, create_coord(0, 1, 0));
	curPos = create_coord(0, 0, 0);
	for (y = startPos.y; y <= maxFull.y + 1; y++)
	{
		for (z = startPos.z; z <= maxFull.z; z++)
		{
			region_t next_row = make_region(create_coord(startPos.x, y-1, z), create_coord(maxFull.x, y-1, z));
			if (region_is_empty(mdl, next_row)) continue;
			for (x = startPos.x; x <= maxFull.x; x++)
			{
				if (get_voxel(mdl, create_coord(x, y-1, z)) == Full)
				{
					if(!is_coords_equal(curPos, create_coord(x, y, z))){
						goto_next_pos(&curPos, create_coord(x, y, z), cmds);
					}
					add_cmd(cmds, fill_cmd(create_coord(0, -1, 0)));
				}
			}
		}
	}

	// go back to home position
	goto_next_pos(&curPos, create_coord(0, maxFull.y + 1, 0), cmds);
	
	goto_next_pos(&curPos, create_coord(0, 0, 0), cmds);

	// turn on gravity
	add_cmd(cmds, flip_cmd());

	// halt
	add_cmd(cmds, halt_cmd());

	return cmds;
}
