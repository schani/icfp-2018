#include "model.h"
#include "state.h"
#include "plan.h"
#include "execution.h"
#include "default_trace.h"
#include "move_helper.h"


bool
contains_bot (state_t state, coord_t pos)
{
	for (int i = 0; i < state.n_bots; ++i)
	{
		if (is_coords_equal(pos, state.bots[i].pos))
			return true;
	}
	return false;
}




// each element in starting_pos will be the starting pos for the respective bot
void
find_starting_points (state_t state, matrix_t partitioning, xyz_t y, coord_t *starting_pos) {
    coord_t dummy_coord = create_coord(-1, -1, -1);
    xyz_t res = state.matrix.resolution;
    int n_bots = state.n_bots;
    // indexed by partition, bot
    coord_t closest_coords[n_bots][n_bots];
    for (int i = 0; i < n_bots; i++) {
        starting_pos[i] = dummy_coord;
        for (int j = 0; j < n_bots; j++) {
            closest_coords[i][j] = dummy_coord;
        }
    }

    for (xyz_t x = 0; x < res; x++) {
        for (xyz_t z = 0; z < res; z++) {
            coord_t c = create_coord(x, y, z);
            int p = get_voxel(&partitioning, c);
            assert(p < n_bots);
            if (p == 0) continue;

            for (int b = 0; b < n_bots; b++) {
                coord_t bot_pos = state.bots[b].pos;
                int dist = get_mdist(c, bot_pos);
                coord_t closest = closest_coords[p][b];
                if (closest.x < 0 || dist < get_mdist(closest, bot_pos)) {
                    closest_coords[p][b] = c;
                }
            }
        }
    }

    for (int i = 0; i < n_bots; i++) {
        int best_p = -1;
        int best_b = -1;
        int best_dist = -1;
        for (int p = 0; p < n_bots; p++) {
            for (int b = 0; b < n_bots; b++) {
                if (starting_pos[b].x >= 0) continue;
                coord_t c = closest_coords[p][b];
                assert(is_coord_valid(&partitioning, c) && get_voxel(&partitioning, c) == p);
                int dist = get_mdist(c, state.bots[b].pos);

                if (best_dist < 0 || dist < best_dist) {
                    best_p = p;
                    best_b = b;
                    best_dist = dist;
                }
            }
        }

        assert(best_dist >= 0 && best_p >= 0 && best_b >= 0);
        assert(starting_pos[best_b].x < 0);

        starting_pos[best_b] = closest_coords[best_p][best_b];
    }

    for (int i = 0; i < n_bots; i++) {
        assert(starting_pos[i].x >= 0);
    }
}

static int
find_grounded (matrix_t *model, int y, matrix_t *phases, matrix_t *blobs, int phase, int blob, matrix_t *grounded) {
    region_t full_region = matrix_region(model);
    resolution_t res = phases->resolution;
    int direction = direction_for_phase(phase);
    int num_grounded = 0;

    *grounded = make_matrix(res);

    FOR_EACH_COORD_XZ(c, y, full_region) {
        if (get_voxel(model, c) != Empty) continue;
        if (get_voxel(phases, c) != phase) continue;
        if (get_voxel(blobs, c) != blob) continue;

        if (is_grounded(model, c, direction)) {
            set_voxel(grounded, c, Full);
            num_grounded++;
        }
    } END_FOR_EACH_COORD_XZ;

    return num_grounded;
}

static void
copy_non_empty (matrix_t *model, matrix_t *grounded) {
    region_t r = matrix_region(model);
    FOR_EACH_COORD(c, r) {
        voxel_t v = get_voxel(grounded, c);
        if (v != Empty) {
            set_voxel(model, c, v);
        }
    } END_FOR_EACH_COORD;
}

static bool
is_cross_point (coord_t c) {
    if ((c.x % 2) == 0) {
        return (c.z % 4) == 1;
    } else {
        return (c.z % 4) == 3;
    }
}

static bool
needs_fill (state_t *state, matrix_t *grounded, coord_t c) {
    if (!is_coord_valid(grounded, c)) return false;
    return get_voxel(grounded, c) != Empty && get_voxel(&state->matrix, c) == Empty;
}

static bool
needs_cross_fill (state_t *state, matrix_t *grounded, coord_t c) {
    if (needs_fill(state, grounded, c)) return true;
    if (needs_fill(state, grounded, add_x(c, -1))) return true;
    if (needs_fill(state, grounded, add_x(c, 1))) return true;
    if (needs_fill(state, grounded, add_z(c, -1))) return true;
    if (needs_fill(state, grounded, add_z(c, 1))) return true;
    return false;
}

static coord_t
get_closest_coord (state_t *state, int y, matrix_t *grounded) {
    coord_t center = create_coord(state->bots[0].pos.x, y, state->bots[0].pos.z);
    int dist = 0;
    for (;;) {
        assert(dist < grounded->resolution * 2);
        coord_t dist_coord = create_coord(dist, 0, dist);
        region_t r = make_region(add_coords(center, dist_coord), sub_coords(center, dist_coord));
        FOR_EACH_COORD_XZ(c, y, r) {
            if (!is_coord_valid(grounded, c)) continue;
            if (get_mdist(center, c) != dist) continue;
            if (!is_cross_point(c)) continue;
            if (!needs_cross_fill(state, grounded, c)) continue;
            return c;
        } END_FOR_EACH_COORD_XZ;
        dist++;
    }
}

static void
exec_cmd (state_t *state, command_t cmd) {
    print_cmd(cmd);
    *state = exec_timestep(*state, &cmd);
}

static void
goto_pos (state_t *state, coord_t next_pos) {
    assert(state->n_bots == 1);
    coord_t cur_pos = state->bots[0].pos;
    GArray *cmds = g_array_new(FALSE, FALSE, sizeof(command_t));
	goto_next_pos(&cur_pos, next_pos, cmds);
    for (int i = 0; i < cmds->len; i++) {
        exec_cmd(state, g_array_index(cmds, command_t, i));
    }
    g_array_free(cmds, TRUE);
}

static int
cross_fill (state_t *state, matrix_t *grounded, int direction, coord_t bot_pos) {
    int num_filled = 0;
    coord_t c = add_y(bot_pos, -direction);
    if (needs_fill(state, grounded, c)) {
        exec_cmd(state, fill_cmd(create_coord(0, -direction, 0)));
        num_filled++;
    }
    if (needs_fill(state, grounded, add_x(c, -1))) {
        exec_cmd(state, fill_cmd(create_coord(-1, -direction, 0)));
        num_filled++;
    }
    if (needs_fill(state, grounded, add_x(c, 1))) {
        exec_cmd(state, fill_cmd(create_coord(1, -direction, 0)));
        num_filled++;
    }
    if (needs_fill(state, grounded, add_z(c, -1))) {
        exec_cmd(state, fill_cmd(create_coord(0, -direction, -1)));
        num_filled++;
    }
    if (needs_fill(state, grounded, add_z(c, 1))) {
        exec_cmd(state, fill_cmd(create_coord(0, -direction, 1)));
        num_filled++;
    }
    return num_filled;
}

static void
fill_grounded (state_t *state, int direction, int y, matrix_t *grounded, int num_grounded) {
    int num_filled = 0;
    region_t full_region = matrix_region(grounded);
    assert(state->n_bots == 1);
    assert(direction == 1);

    while (num_filled < num_grounded) {
        coord_t closest = add_y(get_closest_coord(state, y, grounded), direction);
        // printf("filling from %d %d %d\n", closest.x, closest.y, closest.z);
        goto_pos(state, closest);
        num_filled += cross_fill(state, grounded, direction, closest);
    }
}

void
build_phase (state_t *state, matrix_t *phases, matrix_t *blobs, int phase) {
    resolution_t res = phases->resolution;

    int direction = direction_for_phase(phase);
    int start = direction > 0 ? 0 : res - 1;
    int end = direction > 0 ? res : -1;

    for (int y = start; y != end; y += direction) {
        int blob = 1;
        int num_grounded_in_blob = 0;
        for (;;) {
            matrix_t grounded;
            int num_grounded = find_grounded(&state->matrix, y, phases, blobs, phase, blob, &grounded);
            // printf("found %d grounded at y=%d in blob %d\n", num_grounded, y, blob);

            num_grounded_in_blob += num_grounded;
            if (num_grounded == 0) {
                if (num_grounded_in_blob == 0) {
                    break;
                }
                blob += 1;
                num_grounded_in_blob = 0;
                continue;
            }

            fill_grounded(state, direction, y, &grounded, num_grounded);

            free_matrix(&grounded);
        }
    }
}

// needs some space around origin
void
duplicate_bots (state_t *state)
{
	// FIXME: not optimal, better to track seeds in code!
	
	goto_pos(state, create_coord(0,0,0));

	// round 1
        exec_cmd(state, fission_cmd(create_coord(1,0,0), 2));

	// reporduction rounds 2..5
	for (int i = 2; i < 5; ++i)
	{
		exec_cmd(state, fission_cmd(create_coord(1,0,0), 2));
		for (int j = 2; j <= i; ++j)
			goto_pos(state, create_coord(1,0,0));
	}

	// round 6
	for (int j = 1; j <= 5; ++j)
	{
		exec_cmd(state, fission_cmd(create_coord(0,1,0), 1));
	}

	// round 7
	for (int j = 1; j <= 10; ++j)
	{
		exec_cmd(state, fission_cmd(create_coord(0,0,1), 0));
	}
}

int
main (int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s MODEL\n", argv[0]);
    }

    matrix_t model = read_model_file(argv[1]);
    matrix_t phases, blobs;

    make_plan(&model, &phases, &blobs);

    state_t state = make_start_state(model.resolution);

    build_phase(&state, &phases, &blobs, 1);

    goto_pos(&state, create_coord(0, state.bots[0].pos.y, 0));
    goto_pos(&state, create_coord(0, 0, 0));
    exec_cmd(&state, halt_cmd());

    assert(are_matrixes_equal(model, state.matrix));

    return 0;
}
