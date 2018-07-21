#ifndef _TRACE_H
#define _TRACE_H

#include <stdint.h>
#include "coord.h"
#include "commands.h"

typedef struct {
    coord_t c_min, c_max;
} region_t;

region_t make_region (coord_t c1, coord_t c2);

#define FOR_EACH_COORD(c, r) \
    { \
        coord_t c; \
        for (xyz_t _y = r.c_min.y; _y <= r.c_max.y; _y++) { \
            for (xyz_t _x = r.c_min.x; _x <= r.c_max.x; _x++) { \
                for (xyz_t _z = r.c_min.z; _z <= r.c_max.z; _z++) { \
                    c.x = _x; c.y = _y; c.z = _z;

#define END_FOR_EACH_COORD \
                } \
            } \
        } \
    }


typedef struct {
    int n_commands;
    command_t *commands;
} trace_t;

#endif /* _TRACE_H */
