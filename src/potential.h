#ifndef _POTENTIAL_H
#define _POTENTIAL_H

#include "model.h"
#include "coord.h"
#include "plan.h"
#include <gmodule.h>


int 
calc_potential_simple(matrix_t * m_grounded, matrix_t * phases, matrix_t * blobs, int n_blob, int n_phase, int y);
int 
calc_num_grounded(matrix_t * m_grounded, matrix_t * phases, matrix_t * blobs, int n_blob, int n_phase, int y);
GArray * 
get_frontier(matrix_t * m_grounded, matrix_t * phases, matrix_t * blobs, int n_blob, int n_phase, int y);
int calc_potential(matrix_t * m_grounded, matrix_t * phases, matrix_t * blobs, int n_blob, int n_phase, int y);

#endif /* _POTENTIAL_H */