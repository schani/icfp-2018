/* exists to check all headers */

#include <stdio.h>
#include <glib.h>
#include <gc.h>

#include "model.h"
#include "trace.h"
#include "plan.h"


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
    set_voxel(&testModel, create_coord(1,1,1), Full);
    set_voxel(&testModel, create_coord(1,1,2), Full);
    set_voxel(&testModel, create_coord(2,1,2), Full);
    set_voxel(&testModel, create_coord(2,1,1), Full);
    // filled rectangle #2
    set_voxel(&testModel, create_coord(4,1,4), Full);
    set_voxel(&testModel, create_coord(5,1,4), Full);
    set_voxel(&testModel, create_coord(6,1,4), Full);
    set_voxel(&testModel, create_coord(6,1,5), Full);
    set_voxel(&testModel, create_coord(6,1,6), Full);
    set_voxel(&testModel, create_coord(5,1,6), Full);
    set_voxel(&testModel, create_coord(4,1,6), Full);
    set_voxel(&testModel, create_coord(4,1,5), Full);
    set_voxel(&testModel, create_coord(5,1,5), Full);
        
    matrix_t phases = make_matrix(res);
    matrix_t blobs = make_matrix(res);
    matrix_t grounded = make_matrix(res);
    make_plan(&testModel, &phases, &blobs);
    make_grounded(&phases, &grounded);

    region_t r = matrix_region(&grounded);
    FOR_EACH_COORD(c, r) {
        printf("Coordinate (%i,%i,%i) --> %d\n", c.x, c.y, c.z, get_voxel(&grounded, c)); 
    } END_FOR_EACH_COORD;

    return 0;
}

