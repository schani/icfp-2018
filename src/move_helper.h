#ifndef __MOVE_HELPER_H
#define __MOVE_HELPER_H


#include "coord.h"
#include <gmodule.h>

void goto_next_pos(coord_t *curPos, coord_t nextPos, GArray *cmds);


#endif  // __MOVE_HELPER_H