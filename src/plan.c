#include "model.h"
#include "plan.h"

static bool
is_filled (matrix_t *plan, coord_t c) {
    if (!is_coord_valid(plan, c)) return false;
    return get_voxel(plan, c) != 0;
}

static bool
has_filled_neighbors (matrix_t *plan, coord_t c) {
    if (is_filled(plan, add_x(c, -1))) return true;
    if (is_filled(plan, add_x(c, 1))) return true;
    if (is_filled(plan, add_z(c, -1))) return true;
    if (is_filled(plan, add_z(c, 1))) return true;
    return false;
}

static bool
can_fill(matrix_t *phases, matrix_t *model, coord_t c) {
    if (get_voxel(model, c) == Empty) return false;
    if (get_voxel(phases, c) != 0) return false;
    return true;
}

static bool
fill (matrix_t *phases, matrix_t *blobs, matrix_t *model, coord_t c, voxel_t phase, voxel_t blob) {
    if (!can_fill(phases, model, c)) return false;

    assert(get_voxel(blobs, c) == 0);

    set_voxel(phases, c, phase);
    set_voxel(blobs, c, blob);

    coord_t d;

    d = add_x(c, -1);
    if (is_coord_valid(phases, d)) fill(phases, blobs, model, d, phase, blob);

    d = add_x(c, 1);
    if (is_coord_valid(phases, d)) fill(phases, blobs, model, d, phase, blob);

    d = add_z(c, -1);
    if (is_coord_valid(phases, d)) fill(phases, blobs, model, d, phase, blob);

    d = add_z(c, 1);
    if (is_coord_valid(phases, d)) fill(phases, blobs, model, d, phase, blob);

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

bool
is_grounded (matrix_t *model, coord_t c, int direction) {
    resolution_t res = model->resolution;
    xyz_t y = c.y;
    return y == 0 || (y - direction < res && get_voxel(model, add_y(c, -direction)) != 0) || has_filled_neighbors(model, c);
}

void
make_plan (matrix_t *model, matrix_t *phases, matrix_t *blobs) {
    region_t full_region = matrix_region(model);
    resolution_t res = model->resolution;
    *phases = make_matrix(res);
    *blobs = make_matrix(res);
    int phase = 1;

    int direction = 1;
    for (;;) {
        bool did_fill = false;
        int start = direction > 0 ? 0 : res - 1;
        int end = direction > 0 ? res : -1;
        for (int y = start; y != end; y += direction) {
            int num_blobs = 0;
            FOR_EACH_COORD_XZ(c, y, full_region) {
                if (is_grounded(phases, c, direction) && can_fill(phases, model, c)) {
                    num_blobs++;
                    fill(phases, blobs, model, c, phase, num_blobs);
                    did_fill = true;
                }
            } END_FOR_EACH_COORD_XZ;
            if (num_blobs > 0) {
                fprintf(stderr, "%d fills at y=%d in phase %d\n", num_blobs, y, phase);
            }
        }
        if (!did_fill) {
            assert(plan_fills_model(phases, model));
            fprintf(stderr, "%d phases\n", phase - 1);
            return;
        }

        phase++;
        direction = -direction;
    }
}


void
make_grounded (matrix_t *phases, matrix_t* grounded) {
    region_t r = matrix_region(phases);
    resolution_t res = phases->resolution;
    *grounded = make_matrix(res);

    FOR_EACH_COORD(c, r) {
        if (get_voxel(phases, c) == 0) {
            set_voxel(grounded, c, Free);
            continue;
        }
        
        // all remaining filled phase-areas are by default = TransitiveGrounded
        set_voxel(grounded, c, TransitiveGrounded);

        if (get_voxel(phases, c) % 2 != 0)
        {
            // ODD: compare with layer below
            if (c.y > 0) {
                coord_t below = create_coord(c.x, c.y-1, c.z);
                if (get_voxel(phases, below) > 0 && get_voxel(phases, below)<=get_voxel(phases, c)) {
                    // =filled
                    set_voxel(grounded, c, Grounded);
                }
            }
            else {
                set_voxel(grounded, c, Grounded);
            }
        }
        else 
        {
            // EVEN: compare with upper layer
            if (c.y < r.c_max.y) {
                coord_t above = create_coord(c.x, c.y+1, c.z);
                if (get_voxel(phases, above) > 0 && get_voxel(phases, above)<=get_voxel(phases, c)) {
                    // =filled
                    set_voxel(grounded, c, Grounded);
                }
            }
        }

    } END_FOR_EACH_COORD;
}