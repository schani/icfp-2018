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



void
void_a_boundary_box(matrix_t *mdl, region_t* bb, multi_bot_commands_t *mbc){

    xyz_t cur_bot_y = bb->c_max.y+1;

	for (xyz_t y = bb->c_max.y+1; y > bb->c_min.y; y++)
	{
    	region_t next_row = make_region(create_coord(bb->c_min.x, y-1, bb->c_min.z), create_coord(bb->c_max.x, y-1, bb->c_max.z));
		if (region_is_empty(mdl, next_row)) continue;

        /* move bots down */

		goto_rel_pos(create_coord(0, y-cur_bot_y, 0), mbc->bot_commands[0].cmds);
		goto_rel_pos(create_coord(0, y-cur_bot_y, 0), mbc->bot_commands[1].cmds);
		goto_rel_pos(create_coord(0, y-cur_bot_y, 0), mbc->bot_commands[2].cmds);
		goto_rel_pos(create_coord(0, y-cur_bot_y, 0), mbc->bot_commands[3].cmds);

        set_bot_pos(&mbc->bot_commands[0], add_coords(get_bot_pos(&mbc->bot_commands[0]), create_coord(0, y-cur_bot_y, 0)));
        set_bot_pos(&mbc->bot_commands[1], add_coords(get_bot_pos(&mbc->bot_commands[1]), create_coord(0, y-cur_bot_y, 0)));
        set_bot_pos(&mbc->bot_commands[2], add_coords(get_bot_pos(&mbc->bot_commands[2]), create_coord(0, y-cur_bot_y, 0)));
        set_bot_pos(&mbc->bot_commands[3], add_coords(get_bot_pos(&mbc->bot_commands[3]), create_coord(0, y-cur_bot_y, 0)));

        cur_bot_y = y;

        /* void the region below */
		add_cmd(mbc->bot_commands[0].cmds, gvoid_cmd(create_coord(0, -1, 0), create_coord(bb->c_max.x, 0, bb->c_max.z)));
		add_cmd(mbc->bot_commands[1].cmds, gvoid_cmd(create_coord(0, -1, 0), create_coord(bb->c_max.x, 0, bb->c_min.z)));
		add_cmd(mbc->bot_commands[2].cmds, gvoid_cmd(create_coord(0, -1, 0), create_coord(bb->c_min.x, 0, bb->c_min.z)));
		add_cmd(mbc->bot_commands[3].cmds, gvoid_cmd(create_coord(0, -1, 0), create_coord(bb->c_min.x, 0, bb->c_max.z)));
        
	}

}



GArray* 
exec_test_bb_flush(matrix_t *mdl, bot_t *bot1){


    

}







