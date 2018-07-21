#include "model.h"
#include "default_trace.h"

#include <stdio.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        assert(false);
    }
    matrix_t m = read_model_file(argv[1]);
    int r = m.resolution - 1;

    coord_t origin = create_coord(0,0,0);
    region_t x_0 = make_region(origin, create_coord(0,r,r));
    assert(region_is_empty(&m, x_0));
    region_t x_r = make_region(create_coord(r,0,0), create_coord(r,r,r));
    assert(region_is_empty(&m, x_r));
    region_t z_0 = make_region(origin, create_coord(r,r,0));
    assert(region_is_empty(&m, z_0));
    region_t z_r = make_region(create_coord(0,0,r), create_coord(r,r,r));
    assert(region_is_empty(&m, z_r));
    region_t y_r = make_region(create_coord(0,r,0), create_coord(r,r,r));
    assert(region_is_empty(&m, y_r));
        
    GArray* cmds = exec_default_trace(&m);

    return m.resolution;
}