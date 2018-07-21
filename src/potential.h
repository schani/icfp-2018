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

int calc_potential(matrix_t * m_grounded, GArray * coords);

#endif /* _POTENTIAL_H */