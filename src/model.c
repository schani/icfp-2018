#include "model.h"

matrix_t
make_matrix(int resolution) {
    matrix_t m;
    m.resolution = resolution;
    m.data = calloc(resolution * resolution * resolution, sizeof(voxel_t));
    return m;
}

void
free_matrix (matrix_t matrix) {
    free(matrix.data);
}
