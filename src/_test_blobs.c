/* exists to check all headers */

#include <stdio.h>
#include <glib.h>
#include <gc.h>

#include "model.h"
#include "trace.h"
#include "plan.h"
#include "potential.h"
#include "area.h"


void fill_rectangle_2d(matrix_t *m, int x_start, int y_fixed, int z_start, int x_len, int z_len) {
    // TODO: check if x_start+x_len <= resolution ...

    for (int x = x_start; x < x_start + x_len; x++) {
        for (int z = z_start; z < z_start + z_len; z++) {
            set_voxel(m, create_coord(x,y_fixed,z), Full);
        }
    }
}

int
main() 
{
    GC_INIT();

    matrix_t testModel;
    testModel.resolution = 10;
    resolution_t res = testModel.resolution;
    voxel_t tmp[10*10*10] = {0};
    testModel.data = tmp;
   
    set_voxel(&testModel, create_coord(1,0,1), Full);
    // filled rectangle #1
    fill_rectangle_2d(&testModel,1,1,1,2,2);
    // filled rectangle #2
    fill_rectangle_2d(&testModel,4,1,4,3,3);
    //connect rectangles
    set_voxel(&testModel, create_coord(2,2,2), Full);
    set_voxel(&testModel, create_coord(3,2,2), Full);
    set_voxel(&testModel, create_coord(4,2,2), Full);
    set_voxel(&testModel, create_coord(4,2,3), Full);
    set_voxel(&testModel, create_coord(4,2,4), Full);


    matrix_t phases = make_matrix(res);
    matrix_t blobs = make_matrix(res);
    matrix_t grounded = make_matrix(res);
    make_plan(&testModel, &phases, &blobs);
    make_grounded(&phases, &grounded);

    int fixed_y = 1;
    int fixed_phase = 1;

    region_t r = matrix_region(&phases);
    FOR_EACH_COORD(c, r) {
        //if (get_voxel(&phases, c) == fixed_phase) {
          //  if (fixed_y == c.y)
                
                printf("Coordinate (%i,%i,%i): [ PHASE | BLOB | GROUNDED ] --> [ %d | %d | %d ]\n", c.x, c.y, c.z, get_voxel(&phases, c), get_voxel(&blobs, c), get_voxel(&grounded, c)); 
                
        //}
    } END_FOR_EACH_COORD;

    // iterate through phases and blobs, calc potential
    
    /*for (int x = 0; x < res; x++) {
        for (int z = 0; z < res; z++) {
            coord_t c = create_coord(x, fixed_y, z);
            if (get_voxel(c, phases, fixed_phase) != 0) { 
                int pot = calc_potential(m_grounded, phases, blobs, int n_blob, int n_phase, int y)
            }
        }
    }*/
    printf("bis hierher 222?\n");

    calc_potential(&grounded, &phases, &blobs, 1, fixed_phase, fixed_y);

    //printf("Potential --> %d\n", ); 

    GArray* a;
    area_t area;
    a = find_areas(&phases, &blobs, 1, 1);
    assert(a->len == 1);
    area =  g_array_index(a, area_t, 0);
    region_t box1 =  make_region(create_coord(1, 1, 1), create_coord(2, 1, 2));
    assert(region_equal(&area.bounding_box, &box1));
    a = find_areas(&phases, &blobs, 1, 2);
    assert(a->len == 1);
    a = find_areas(&phases, &blobs, 2, 1);
    assert(a->len == 1);

    return 0;
}

