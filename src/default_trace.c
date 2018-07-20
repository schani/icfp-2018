#include <gmodule.h>

#include "trace.h"
#include "model.h"

void set_coords(coord_t *c, int16_t x, int16_t y, int16_t z)
{
	c->x = x;
	c->y = y;
	c->z = z;
}

// FIXME: rename to make_coord and make public
coord_t create_coord(int16_t x, int16_t y, int16_t z)
{
	coord_t c;
	c.x = x;
	c.y = y;
	c.z = z;
	return c;
}

void calc_boundary_box(matrix_t *mdl, coord_t *minFull, coord_t *maxFull)
{
	uint8_t x = 0, y = 0, z = 0;
	coord_t min = create_coord(0, 0, 0);
	coord_t max = create_coord(mdl->resolution, mdl->resolution, mdl->resolution);
	*minFull = max;
	*maxFull = min;
	region_t r = make_region(min, max);

	FOR_EACH_COORD(cur, r) {
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
	} END_FOR_EACH_COORD;
}

void halt_cmd(command_t *cmds)
{
	// add command !
}

command_t fill_cmd(coord_t nd)
{
	command_t cmd;
	cmd.type = Fill;
	// FIXME: check nd
	cmd.Fill_nd = nd;
	return cmd;
}

void smove_cmd(coord_t *curPos, coord_t lld, command_t *cmds)
{
	*curPos = add_coords(*curPos, lld);
	// add command !
}

void goto_next_pos(coord_t *curPos, coord_t nextPos, command_t *cmds)
{
	coord_t tmp;
	coord_t lld;

	// y movement must be done at 1st !
	while ((nextPos.y - curPos->y) > 15)
	{
		set_coords(&lld, 0, 15, 0);
		smove_cmd(curPos, lld, cmds);
	}
	set_coords(&lld, 0, (nextPos.y - curPos->y), 0);
	smove_cmd(curPos, lld, cmds);

	while ((nextPos.x - curPos->x) > 15)
	{
		set_coords(&lld, 15, 0, 0);
		smove_cmd(curPos, lld, cmds);
	}
	set_coords(&lld, (nextPos.x - curPos->x), 0, 0);
	smove_cmd(curPos, lld, cmds);

	while ((nextPos.z - curPos->z) > 15)
	{
		set_coords(&lld, 0, 0, 15);
		smove_cmd(curPos, lld, cmds);
	}
	set_coords(&lld, 0, 0, (nextPos.z - curPos->z));
	smove_cmd(curPos, lld, cmds);
}

static void
add_cmd (GArray *cmds, command_t cmd) {
	g_array_append_val(cmds, cmd);
}

GArray exec_default_trace(matrix_t *mdl)
{
	GArray* cmds = g_array_new(FALSE, FALSE, sizeof(command_t));

	// Calculate boundary box
	coord_t minFull, maxFull;
	calc_boundary_box(mdl, &minFull, &maxFull);

	coord_t curPos, startPos;
	set_coords(&curPos, 0, 0, 0);
	int16_t x, y, z;

	startPos = add_coords(minFull, create_coord(0, 1, 0));
	for (y = startPos.y; y < maxFull.y + 1; y++)
	{
		for (z = startPos.z; z < maxFull.z; z++)
		{
			for (x = startPos.x; x < maxFull.x; x++)
			{
				goto_next_pos(&curPos, create_coord(x, y, z), cmds);
				if (get_voxel(mdl, curPos) == Full)
				{
					add_cmd(cmds, fill_cmd(create_coord(0, -1, 0)));
				}
			}
		}
	}
	// go back to home position
	goto_next_pos(&curPos, create_coord(0, maxFull.y + 1, 0), cmds);
	goto_next_pos(&curPos, create_coord(0, 0, 0), cmds);

	halt_cmd(cmds);
}
