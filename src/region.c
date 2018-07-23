#include "region.h"

#include <glib.h>

region_t
extend_region(region_t r_in, coord_t c) {
    region_t r;
    r.c_min.x = MIN(r_in.c_min.x, c.x);
    r.c_max.x = MAX(r_in.c_max.x, c.x);
    r.c_min.y = MIN(r_in.c_min.y, c.y);
    r.c_max.y = MAX(r_in.c_max.y, c.y);
    r.c_min.z = MIN(r_in.c_min.z, c.z);
    r.c_max.z = MAX(r_in.c_max.z, c.z);
    return r;
}

region_t
make_region (coord_t c1, coord_t c2) {
    region_t r;
    r.c_min.x = MIN(c1.x, c2.x);
    r.c_max.x = MAX(c1.x, c2.x);
    r.c_min.y = MIN(c1.y, c2.y);
    r.c_max.y = MAX(c1.y, c2.y);
    r.c_min.z = MIN(c1.z, c2.z);
    r.c_max.z = MAX(c1.z, c2.z);
    return r;
}

bool
region_equal(region_t* r1, region_t* r2) {
    return (
        r1->c_min.x == r2->c_min.x &&
        r1->c_min.y == r2->c_min.y &&
        r1->c_min.z == r2->c_min.z &&
        r1->c_max.x == r2->c_max.x &&
        r1->c_max.y == r2->c_max.y &&
        r1->c_max.z == r2->c_max.z
    );
}

region_t get_grid_region(region_grid_t grid, int row, int col) {
    assert(grid.rows > row);
    assert(grid.cols > col);
    return grid.regions[row + col * grid.rows];
}

static inline
void set_grid_region(region_grid_t grid, int row, int col, region_t r) {
    assert(grid.rows > row);
    assert(grid.cols > col);
    grid.regions[row + col * grid.rows] = r;
}

region_grid_t split_region(region_t r, int rows, int cols) {
    region_grid_t grid;
    grid.rows = rows;
    grid.cols = cols;
    grid.regions = calloc(sizeof(region_t), rows*cols);

    xyz_t dim_x = 1 + r.c_max.x - r.c_min.x;
    xyz_t dim_z = 1 + r.c_max.z - r.c_min.z;

    xyz_t width_x = dim_x / cols;
    xyz_t width_z = dim_z / rows;
    
    xyz_t missing_x = dim_x - cols*width_x; 
    xyz_t missing_z = dim_z - rows*width_z;

    xyz_t height = r.c_max.y - r.c_min.y; 

    xyz_t x_offset = r.c_min.x;
    for (int col = 0; col < cols; col++) {
        xyz_t x_curr_width = width_x;
        if(missing_x > 0){
            x_curr_width += 1;
            missing_x -=1;
        }
        //xyz_t x_curr_width = width_x + missing_x;
        //missing_x = 0;

        xyz_t curr_missing_z = missing_z;
        xyz_t z_offset = r.c_min.z;
        for (int row = 0; row < rows; row++) {
            xyz_t z_curr_width = width_z;
            if(curr_missing_z > 0){
                z_curr_width +=1;
                curr_missing_z -=1;
            }
            coord_t c_min = create_coord(x_offset, r.c_min.y, z_offset);
            coord_t c_max = add_coords(c_min, create_coord(x_curr_width-1, height, z_curr_width-1));
            set_grid_region(grid, row, col, make_region(c_min, c_max));
            z_offset += z_curr_width;
       }
       x_offset += x_curr_width;
    }
    return grid;
}

static int
test() {
    region_t r = make_region(create_coord(1,0,1), create_coord(28,28,28));
    region_grid_t grid = split_region(r, 3, 2);
    region_t r00 = get_grid_region(grid, 0, 0);
    region_t r10 = get_grid_region(grid, 1, 0);
    region_t r20 = get_grid_region(grid, 2, 0);
    region_t r01 = get_grid_region(grid, 0, 1);
    region_t expected00 = make_region(create_coord( 1, 0,  1), create_coord(14,28,10));
    region_t expected10 = make_region(create_coord( 1, 0, 11), create_coord(14,28,19));
    region_t expected20 = make_region(create_coord( 1, 0, 20), create_coord(14,28,28));
    region_t expected01 = make_region(create_coord(15, 0,  1), create_coord(28,28,10));
    assert(region_equal(&r00, &expected00));
    assert(region_equal(&r10, &expected10));
    assert(region_equal(&r20, &expected20));
    assert(region_equal(&r01, &expected01));
}
