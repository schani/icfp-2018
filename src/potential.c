

#include "potential.h"




/* calcs the number of grounded voxels */
int 
calc_num_grounded(GArray * coords){
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


static inline
grounded_type_t get_grounded(matrix_t * m, coord_t c){
    return get_voxel( m, c);
}





GArray * get_frontier(matrix_t * m_grounded, GArray * coords){
    GArray *frontier = g_array_new(FALSE, FALSE, sizeof(extd_coord_t));
    int i, j;
    /* iterate over area voxels */
    for (i=0; i<coords->len; i++) {
        extd_coord_t coord = (extd_coord_t)g_array_index(coords, extd_coord_t, i);
        /* check for not directly grounded voxels */
        if (coord.grounded == TransitiveGrounded){
            GArray *  neighbors = get_planar_neighbor_voxels(m_grounded, coord.c);
            /* iterate over neighbors */
            for (j=0; j<neighbors->len; j++) {
                coord_t ncoord = (coord_t)g_array_index(neighbors, coord_t, j);
                if(get_grounded(m_grounded, ncoord) == Grounded){
                    /* direct neighbor is grounded so current voxel is frontier */
                    g_array_append_val(frontier, coord);
                    break;
                }
            }
            g_array_free(neighbors, TRUE);
        }
    }
    assert(false);
}

int 
calc_potential_grounded_plus_frontier(matrix_t * m_grounded, GArray * coords){
    int pot = 0;

    /* add grounded elements */
    pot += calc_num_grounded(coords);
    /* add size of the frontier */
    GArray * frontier = get_frontier(m_grounded, coords);
    pot += frontier->len;

    g_array_free(frontier, TRUE);

    return pot;
}


/* calculates the potential of a area only by the number of voxels*/
int 
calc_potential_simple(GArray *coords){
    return coords->len;
}





int calc_potential(matrix_t * m_grounded, GArray * coords){

    return calc_potential_grounded_plus_frontier(m_grounded, coords);
}



