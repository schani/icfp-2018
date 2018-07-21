/* exists to check all headers */

#include <stdio.h>
#include <glib.h>
#include <gc.h>

#include "model.h"
#include "trace.h"
#include "plan.h"
#include "potential.h"
#include "area.h"
#include "bot_plan.h"


void fill_rectangle_2d(matrix_t *m, int x_start, int y_fixed, int z_start, int x_len, int z_len) {
    // TODO: check if x_start+x_len <= resolution ...

    // Question: why is this x_len and not x_len + 1?
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

    // hacking together semivalid state    
    state_t state = make_state(0, Low, make_matrix(testModel.resolution));
    state.bots = malloc(sizeof(bot_t*)* 2);
    state.bots[0] = make_bot (0, create_coord(0,1,0), 0, NULL);
    state.bots[1] = make_bot (1, create_coord(testModel.resolution-1, 1, testModel.resolution-1), 0, NULL);
    state.n_bots = 2;
    state.matrix = testModel;

    // iterate through phases and blobs, calc potentialassigning to type ‘matrix_t
    for (int y=0; y<=max_y; y++) {
        GArray* areas = find_areas(&phases, &blobs, fixed_phase, y);
        for (int a=0; a<areas->len; a++) {
            area_t area = g_array_index(areas, area_t, a);
            int pot = calc_potential(&grounded, &phases, &blobs, a+1, fixed_phase, y);
            area.potential = pot;
            printf("Potential for y-Layer %d / Blob %d : %d\n", y, a+1, pot); 
        }

        bot_partition_t partition = partition_bots(areas, &state);

        if (y == 1) {
            assert(partition.n_areas == 2);
            assert(partition.area_n_bots[0] == 1);
            assert(partition.area_n_bots[1] == 1);
            assert(partition.n_bots == 2);
        } else {
            assert(partition.n_areas == 1);
            assert(partition.area_n_bots[0] == 2);
            assert(partition.n_bots == 2);
        }
    }

    GArray* a;
    area_t area;
    a = find_areas(&phases, &blobs, 1, 1);
    assert(a->len == 2);

    region_t box1 =  make_region(create_coord(4, 1, 2), create_coord(5, 1, 5));
    region_t box2 =  make_region(create_coord(1, 1, 3), create_coord(1, 1, 5));

    area =  g_array_index(a, area_t, 0);
    assert(region_equal(&area.bounding_box, &box1) || region_equal(&area.bounding_box, &box2));

    area = g_array_index(a, area_t, 1);
    assert(region_equal(&area.bounding_box, &box1) || region_equal(&area.bounding_box, &box2));

    return 0;
}

