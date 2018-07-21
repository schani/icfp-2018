#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "coord.h"
#include "model.h"
#include "trace.h"


matrix_t
make_matrix(resolution_t resolution)
{
    matrix_t m;
    m.resolution = resolution;
    m.data = calloc(resolution * resolution * resolution, sizeof(voxel_t));
    return m;
}

matrix_t
copy_matrix(matrix_t from)
{
    matrix_t m = make_matrix(from.resolution);
    memcpy(m.data, from.data, from.resolution * from.resolution * from.resolution * sizeof(voxel_t));
    return m;
}

bool
are_matrixes_equal(matrix_t a, matrix_t b) {
    if (a.resolution != b.resolution) return false;
    region_t r = matrix_region(&a);
    FOR_EACH_COORD(c, r) {
        if (get_voxel(&a, c) != get_voxel(&b, c)) return false;
    } END_FOR_EACH_COORD;
    return true;
}

void
free_matrix (matrix_t matrix) {
    free(matrix.data);
}

bool
region_is_empty (matrix_t *m, region_t r) {
    FOR_EACH_COORD(c, r) {
        if (get_voxel(m, c) != Empty)
            return false;
    } END_FOR_EACH_COORD
    return true;
}

#include <stdio.h>

matrix_t
read_model_file(char* filename) {
    struct stat stats;
    int fd = open(filename, O_RDONLY);
    int status = fstat (fd, &stats);
    assert(status == 0);
    uint8_t* file = mmap(NULL, stats.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(file != MAP_FAILED);

    xyz_t x = 0, y = 0, z = 0;
    coord_t c;
    resolution_t resolution = file[0];
    matrix_t m = make_matrix(resolution);

    for (int offset = 1; offset < stats.st_size; offset++) {
        uint8_t byte = file[offset];
        for (int bit = 0; bit < 8; bit++) {
            c = create_coord(x, y, z);
            assert(is_coord_valid(&m, c));
            uint8_t mask = 1<< bit;
            set_voxel(&m, c, (byte & mask)?Full:Empty);
            z++;
            y += (z/resolution);
            x += (y/resolution);
            y = y % resolution;
            z = z % resolution;
            if (x == resolution) {
                return m;
            }
        }
    }
    assert(false);
}
