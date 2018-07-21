#include "model.h"

static bool
has_filled_neighbors (matrix_t *plan, coord_t c) {
    if (get_voxel(plan, add_x(c, -1)) != 0) return true;
    if (get_voxel(plan, add_x(c, 1)) != 0) return true;
    if (get_voxel(plan, add_z(c, -1)) != 0) return true;
    if (get_voxel(plan, add_z(c, 1)) != 0) return true;
    return false;
}

static bool
fill (matrix_t *plan, matrix_t *model, coord_t c, voxel_t phase) {
    if (get_voxel(model, c) == Empty) return false;
    if (get_voxel(plan, c) != 0) return false;

    set_voxel(plan, c, phase);

    coord_t d;

    d = add_x(c, -1);
    if (is_coord_valid(plan, d)) fill(plan, model, d, phase);

    d = add_x(c, 1);
    if (is_coord_valid(plan, d)) fill(plan, model, d, phase);

    d = add_z(c, -1);
    if (is_coord_valid(plan, d)) fill(plan, model, d, phase);

    d = add_z(c, 1);
    if (is_coord_valid(plan, d)) fill(plan, model, d, phase);

    return true;
}

static bool
plan_fills_model (matrix_t *plan, matrix_t *model) {
    assert(plan->resolution == model->resolution);
    region_t r = matrix_region(plan);
    FOR_EACH_COORD(c, r) {
        if ((get_voxel(plan, c) == 0) != (get_voxel(model, c) == Empty)) {
            return false;
        }
    } END_FOR_EACH_COORD;
    return true;
}

matrix_t
make_plan (matrix_t *model) {
    resolution_t res = model->resolution;
    matrix_t plan = make_matrix(res);
    int phase = 1;

    int direction = 1;
    for (;;) {
        bool did_fill = false;
        int start = direction > 0 ? 0 : res - 1;
        int end = direction > 0 ? res : -1;
        for (int y = start; y != end; y += direction) {
            for (int x = 0; x < res; x++) {
                for (int z = 0; z < res; z++) {
                    coord_t c = create_coord(x, y, z);
                    if (y == 0 || get_voxel(&plan, add_y(c, -direction)) != 0 || has_filled_neighbors(&plan, c)) {
                        did_fill = fill(&plan, model, c, phase) || did_fill;
                    }
                }
            }
        }
        if (!did_fill) {
            assert(plan_fills_model(&plan, model));
            printf("%d phases\n", phase);
            return plan;
        }

        phase++;
        direction = -direction;
    }
}