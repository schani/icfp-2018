#include <stdint.h>
#include <stdlib.h>

#include "trace.h"

typedef struct {
    int resolution;
    uint8_t *data;
} model_t;

static size_t
get_pos (model_t *m, coord_t c) {
    return ((c.x * m->resolution) + c.y) * m->resolution + c.z;
}

static inline uint8_t
get_voxel (model_t *m, coord_t c) {
    return m->data[get_pos(m, c)];
}

static inline uint8_t
set_voxel (model_t *m, coord_t c, uint8_t v) {
    m->data[get_pos(m, c)] = v;
}
