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


#endif /* _PLAN_H */
