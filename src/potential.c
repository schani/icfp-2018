

#include "potential.h"











int calc_num_grounded(GArray * coords){
    int num_grounded = 0;
    int i;
    for (i=0; i<coords->len; i++) {
        extd_coord_t coord = (extd_coord_t)g_array_index(coords, extd_coord_t, i);
        if (coord.grounded == Grounded){
            num_grounded += 1;
        }
    }
    return num_grounded;
}


/* calculates the potential of a area only by the number of voxels*/
int calc_potential_simple(GArray *coords){
    return coords->len;
}
