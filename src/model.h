#ifndef _MODEL_H
#define _MODEL_H

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "coord.h"
#include "trace.h"

typedef uint8_t voxel_t;
typedef uint8_t resolution_t;

typedef enum {
    Empty = 0,
    Full = 1
} voxel_state_t;

typedef struct {
    resolution_t resolution;
    voxel_t *data;
} matrix_t;

static bool
is_coord_valid (matrix_t *m, coord_t c) {
    if (c.x < 0 || c.y < 0 || c.z < 0) return false;
    resolution_t r = m->resolution;
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

static inline region_t
matrix_region (matrix_t *m) {
    resolution_t res = m->resolution;
    return make_region(create_coord(0, 0, 0), create_coord(res-1, res-1, res-1));
}

static inline region_t 
layer_region (matrix_t *m, xyz_t y) {
    resolution_t res = m->resolution;
    return make_region(create_coord(0, y, 0), create_coord(res-1, y, res-1));
}

GArray * 
get_planar_neighbor_voxels(matrix_t *m, coord_t c);

bool region_is_empty (matrix_t *m, region_t r);

matrix_t make_matrix(resolution_t resolution);

matrix_t copy_matrix(matrix_t from);

bool are_matrixes_equal(matrix_t a, matrix_t b);

void free_matrix(matrix_t *matrix);

matrix_t read_model_file(char* filename);

#endif /* _MODEL_H */