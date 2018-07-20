#include "model.h"

#define MatrixDataSize(resolution) resolution * resolution * resolution, sizeof(voxel_t)

matrix_t
make_matrix(int resolution) {
    matrix_t m;
    m.resolution = resolution;
    m.data = calloc(MatrixDataSize(resolution);
    return m;
}

matrix_t
copy_matrix(matrix_t* from) {
    matrix_t m = make_matrix(from.resolution);
    memcpy(m.data, from->data, MatrixDataSize(resolution);
    return m;
}
