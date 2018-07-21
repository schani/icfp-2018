#ifndef _AREA_H
#define _AREA_H

#include "coord.h"


typedef struct {
    coord_t c;
    int grounded;
    int potential;
} extd_coord_t;



/**
 * Input: MATRIX, y-Ebene fix
 * Output: Liste an Flächen
 */
GArray* identify_areas(matrix_t *xzArea) {
    
    GArray *coords = g_array_new(FALSE, FALSE, sizeof(extd_coord_t));

    return coords;
}


#endif /* _AREA_H */