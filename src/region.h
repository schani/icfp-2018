#ifndef __REGION_HELPERS_H
#define __REGION_HELPERS_H

#include "coord.h"

typedef struct {
    coord_t c_min, c_max;
} region_t;

#define FOR_EACH_COORD_XZ(c, _y, r) \
    { \
        coord_t c; \
        for (xyz_t _x = r.c_min.x; _x <= r.c_max.x; _x++) { \
            for (xyz_t _z = r.c_min.z; _z <= r.c_max.z; _z++) { \
                c.x = _x; c.y = _y; c.z = _z;

#define END_FOR_EACH_COORD_XZ \
            } \
        } \
    }

#define FOR_EACH_COORD(c, r) \
    for (xyz_t _y = r.c_min.y; _y <= r.c_max.y; _y++) { \
        FOR_EACH_COORD_XZ(c, _y, r) {

#define END_FOR_EACH_COORD \
        } \
    } END_FOR_EACH_COORD_XZ

region_t 
make_region (coord_t c1, coord_t c2);
region_t 
extend_region(region_t r_in, coord_t c);

bool 
region_equal(region_t* r1, region_t* r2);


#endif  // __REGION_HELPERS_H