#ifndef _POTENTIAL_H
#define _POTENTIAL_H

#include "model.h"
#include "coord.h"
#include <gmodule.h>
#include "area_detect.h"

typedef struct{

 matrix_t m;
 GArray * elements;

} area_t;




int calc_potential(matrix_t * m_grounded, matrix_t * phases, matrix_t * blobs, int n_blob, int n_phase, int y);

#endif /* _POTENTIAL_H */