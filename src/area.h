#ifndef _AREA_H
#define _AREA_H

#include "coord.h"
#include "plan.h"

#include <gmodule.h>

// WIP: area
typedef struct {
    region_t bounding_box;
    int phase;
    int blob_id;
    int potential; 
} area_t;

// FIXME: implement
GArray* find_areas(matrix_t* phases, matrix_t* blobs, int phase, int y);


#endif /* _AREA_H */