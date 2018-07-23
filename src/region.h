#ifndef __REGION_HELPERS_H
#define __REGION_HELPERS_H

#include "coord.h"

typedef struct {
    coord_t c_min, c_max;
} region_t;

// a contiguous region split into a grid
typedef struct {
    int rows;
    int cols;
    region_t* regions;
} region_grid_t;



static inline xyz_t 
region_get_xsize(region_t r){
    return r.c_max.x - r.c_min.x + 1;
}
static inline xyz_t 
region_get_ysize(region_t r){
    return r.c_max.y - r.c_min.y + 1;
}
static inline xyz_t 
region_get_zsize(region_t r){
    return r.c_max.z - r.c_min.z + 1;
}




region_t get_grid_region(region_grid_t grid, int row, int col);

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


static inline region_t
calc_region_intersection(region_t r1, region_t r2){
    coord_t cmin = create_coord(MAX(r1.c_min.x, r2.c_min.x), MAX(r1.c_min.y, r2.c_min.y), MAX(r1.c_min.z, r2.c_min.z));
    coord_t cmax = create_coord(MIN(r1.c_max.x, r2.c_max.x), MIN(r1.c_max.y, r2.c_max.y), MIN(r1.c_max.z, r2.c_max.z));
    return make_region(cmin, cmax);
}



bool 
region_equal(region_t* r1, region_t* r2);

region_grid_t split_region(region_t r, int rows, int cols);


#endif  // __REGION_HELPERS_H