#ifndef _MODEL_H
#define _MODEL_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include "coord.h"
#include "trace.h"

typedef uint8_t voxel_t;

typedef enum {
    Empty = 0,
    Full = 1
} voxel_state_t;

typedef struct {
    int resolution;
    voxel_t *data;
} matrix_t;

static bool
is_coord_valid (matrix_t *m, coord_t c) {
    if (c.x < 0 || c.y < 0 || c.z < 0) return false;
    int r = m->resolution;
    if (c.x >= r || c.y >= r || c.z >= r) return false;
    return true;
}

static size_t
get_pos (matrix_t *m, coord_t c) {
    assert(is_coord_valid(m, c));
    return ((c.y * m->resolution) + c.x) * m->resolution + c.z;
}

static inline voxel_t
get_voxel (matrix_t *m, coord_t c) {
    return m->data[get_pos(m, c)];
}

static inline void
set_voxel (matrix_t *m, coord_t c, voxel_t v) {
    m->data[get_pos(m, c)] = v;
}

bool region_is_empty (matrix_t *m, region_t r);

matrix_t make_matrix(int resolution);

matrix_t copy_matrix(matrix_t from);

void free_matrix(matrix_t matrix);

#endif /* _MODEL_H */