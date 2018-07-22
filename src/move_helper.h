#ifndef __MOVE_HELPER_H
#define __MOVE_HELPER_H


#include "coord.h"
#include <gmodule.h>

int goto_next_pos(coord_t *curPos, coord_t nextPos, GArray *cmds);
int goto_rel_pos(coord_t rel_pos, GArray *cmds);


#endif  // __MOVE_HELPER_H