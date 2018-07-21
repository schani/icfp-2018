#include "potential.h"
#include "plan.h"

/* checks if given voxed is active in current phase */
static inline bool
is_voxel_active(coord_t v, matrix_t * phases, matrix_t * blobs, int n_blob, int n_phase){
    return (get_voxel(phases, v) == n_phase && get_voxel(blobs, v) == n_blob);
}

/* calcs the number of grounded voxels */
int 
calc_num_grounded(matrix_t * m_grounded, matrix_t * phases, matrix_t * blobs, int n_blob, int n_phase, int y){
    int num_grounded = 0;
    resolution_t res = m_grounded->resolution;

    int x, z;
    for (int x = 0; x < res; x++) {
        for (int z = 0; z < res; z++) {
            coord_t c = create_coord(x, y, z);
            if (is_voxel_active(c, phases, blobs, n_blob, n_phase)){
                if (get_voxel(m_grounded, c) == Grounded){
                    num_grounded += 1;
                }
            }
        }
    }
    return num_grounded;
}




GArray * 
get_frontier(matrix_t * m_grounded, matrix_t * phases, matrix_t * blobs, int n_blob, int n_phase, int y){
    GArray *frontier = g_array_new(FALSE, FALSE, sizeof(coord_t));
    resolution_t res = m_grounded->resolution;

    int x, z, i;

    for (int x = 0; x < res; x++) {
        for (int z = 0; z < res; z++) {
            coord_t c = create_coord(x, y, z);
            if (is_voxel_active(c, phases, blobs, n_blob, n_phase)){
                if (get_voxel(m_grounded, c) == TransitiveGrounded){
                    GArray *  neighbors = get_planar_neighbor_voxels(m_grounded, c);
                    /* iterate over neighbors */
                    for (i=0; i<neighbors->len; i++) {
                        coord_t ncoord = (coord_t)g_array_index(neighbors, coord_t, i);
                        if(get_voxel(m_grounded, ncoord) == Grounded){
                            /* direct neighbor is grounded so current voxel is frontier */
                            g_array_append_val(frontier, c);
                            break;
                        }
                    }
                    g_array_free(neighbors, TRUE);
                }             
            }
        }
    }
    return frontier;
}

/* more interesting potential function taking into consideration the number of grounded voxels
   plus the frontier to the transitive grounded voxels */
int 
calc_potential_grounded_plus_frontier(matrix_t * m_grounded, matrix_t * phases, matrix_t * blobs, int n_blob, int n_phase, int y){
    int pot = 0;

    /* add grounded elements */
    pot += calc_num_grounded(m_grounded, phases, blobs, n_blob, n_phase, y);
    /* add size of the frontier */
    GArray * frontier = get_frontier(m_grounded, phases, blobs, n_blob, n_phase, y);
    pot += frontier->len;

    g_array_free(frontier, TRUE);

    return pot;
}




/* calculates the potential of a area only by the number of voxels*/
int 
calc_potential_simple(matrix_t * m_grounded, matrix_t * phases, matrix_t * blobs, int n_blob, int n_phase, int y){

    resolution_t res = m_grounded->resolution;
    int num_grounded=0;
    int x, z;

    for (int x = 0; x < res; x++) {
        for (int z = 0; z < res; z++) {
            coord_t c = create_coord(x, y, z);
            if (is_voxel_active(c, phases, blobs, n_blob, n_phase)){
                if (get_voxel(m_grounded, c) == Grounded){
                    num_grounded += 1;
                }             
            }
        }
    }


    return num_grounded;
}


/* wrapper to provide the official potential function */
int calc_potential(matrix_t * m_grounded, matrix_t * phases, matrix_t * blobs, int n_blob, int n_phase, int y){
    //return calc_potential_simple(m_grounded, phases, blobs, n_blob, n_phase, y);
    return calc_potential_grounded_plus_frontier(m_grounded, phases, blobs, n_blob, n_phase, y);
}



