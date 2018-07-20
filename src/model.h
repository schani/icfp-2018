#include <stdint.h>
#include <stdlib.h>

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

static size_t
get_pos (matrix_t *m, coord_t c) {
    return ((c.y * m->resolution) + c.x) * m->resolution + c.z;
}

static inline voxel_t
get_voxel (matrix_t *m, coord_t c) {
    return m->data[get_pos(m, c)];
}

static inline voxel_t
set_voxel (matrix_t *m, coord_t c, voxel_t v) {
    m->data[get_pos(m, c)] = v;
}

matrix_t make_matrix(int resolution);

matrix_t copy_matrix(matrix_t from);

void free_matrix(matrix_t matrix);
