#include "model.h"
#include "memory.h"

matrix_t
make_matrix(int resolution) {
    matrix_t m;
    m.resolution = resolution;
    m.data = calloc(resolution * resolution * resolution, sizeof(voxel_t));
    return m;
}

matrix_t
copy_matrix(matrix_t from) {
    matrix_t m = make_matrix(from.resolution);
    memcpy(m.data, from.data, from.resolution * from.resolution * from.resolution * sizeof(voxel_t));
    return m;
}

void
free_matrix (matrix_t matrix) {
    free(matrix.data);
}
