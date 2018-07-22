#include "default_trace.h"

#include "move_helper.h"

#include <stdio.h>
#include <stdlib.h>


void calc_boundary_box(matrix_t *mdl, coord_t *minFull, coord_t *maxFull)
{
	uint8_t x = 0, y = 0, z = 0;
	coord_t min = create_coord(0, 0, 0);
	coord_t max = create_coord(mdl->resolution-1, mdl->resolution-1, mdl->resolution-1);
	*minFull = max;
	*maxFull = min;
	region_t r = make_region(min, max);

	FOR_EACH_COORD(cur, r)
	{
		if (get_voxel(mdl, cur) == Full)
		{
			if (cur.x < minFull->x)
			{
				minFull->x = cur.x;
			}
			if (cur.y < minFull->y)
			{
				minFull->y = cur.y;
			}
			if (cur.z < minFull->z)
			{
				minFull->z = cur.z;
			}
			if (cur.x > maxFull->x)
			{
				maxFull->x = cur.x;
			}
			if (cur.y > maxFull->y)
			{
				maxFull->y = cur.y;
			}
			if (cur.z > maxFull->z)
			{
				maxFull->z = cur.z;
			}
		}
	}
	END_FOR_EACH_COORD;
}



GArray* exec_default_trace(matrix_t *mdl)
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
