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
   
    //set_voxel(&testModel, create_coord(1,0,1), Full);
    // filled rectangle #1
    //fill_rectangle_2d(&testModel,1,1,1,2,2);
    fill_rectangle_2d(&testModel,1,0,1,4,4);
    fill_rectangle_2d(&testModel,4,1,2,2,4);
    fill_rectangle_2d(&testModel,1,1,3,1,3);
    // filled rectangle #2
    //fill_rectangle_2d(&testModel,4,1,4,3,3);
    //connect rectangles
    /*set_voxel(&testModel, create_coord(2,2,2), Full);
    set_voxel(&testModel, create_coord(3,2,2), Full);
    set_voxel(&testModel, create_coord(4,2,2), Full);
    set_voxel(&testModel, create_coord(4,2,3), Full);
    set_voxel(&testModel, create_coord(4,2,4), Full);*/


    matrix_t phases = make_matrix(res);
    matrix_t blobs = make_matrix(res);
    matrix_t grounded = make_matrix(res);
    make_plan(&testModel, &phases, &blobs);
    make_grounded(&phases, &grounded);

    int fixed_phase = 1;
    int max_y = 0;

    region_t r = matrix_region(&phases);
    FOR_EACH_COORD(c, r) {
        if (get_voxel(&phases, c) > 0) {
        //if (get_voxel(&phases, c) == fixed_phase) {
            printf("Coordinate (%i,%i,%i): [ PHASE | BLOB | GROUNDED ] --> [ %d | %d | %d ]\n", c.x, c.y, c.z, get_voxel(&phases, c), get_voxel(&blobs, c), get_voxel(&grounded, c)); 
            if (c.y > max_y) {
                max_y = c.y;
            }
        }
    } END_FOR_EACH_COORD;

    // iterate through phases and blobs, calc potential
    for (int y=0; y<=max_y; y++) {
        GArray* areas = find_areas(&phases, &blobs, fixed_phase, y);
        for (int a=0; a<areas->len; a++) {
            area_t area = g_array_index(areas, area_t, a);
            int pot = calc_potential(&grounded, &phases, &blobs, a+1, fixed_phase, y);
            area.potential = pot;
            printf("Potential for y-Layer %d / Blob %d : %d\n", y, a+1, pot); 
        }
    }
    

    return 0;
}

