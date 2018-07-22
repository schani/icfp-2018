#include "default_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



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
    GArray * b1_cmds;
    GArray * b2_cmds;
    GArray * b3_cmds;
    GArray * b4_cmds;
} cmd4;



/* assumes the 4 bots are sitting on the top of the bb */

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

    int16_t cur_y;

    return ret_val;

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
