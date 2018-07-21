#ifndef _PLAN_H
#define _PLAN_H

#include "model.h"

typedef enum{  
    Free = 0,
    Grounded,
    TransitiveGrounded
} grounded_type_t;

void make_plan (matrix_t *model, matrix_t *phases, matrix_t *blobs);

void make_grounded (matrix_t *phases, matrix_t* grounded);

bool is_grounded (matrix_t *model, coord_t c, int direction);

static int
direction_for_phase (int phase) {
    return (phase % 2 == 1) ? 1 : -1;
}

#endif /* _PLAN_H */
