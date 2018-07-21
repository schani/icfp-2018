#include "state.h"
#include "model.h"

typedef struct {
    state_t state;
    command_t *commands;
    matrix_t vol;
    GArray *new_bots;
    state_t new_state;
} execution_t;

execution_t start_timestep (state_t state, command_t *commands);
state_t finish_timestep (execution_t *exec);

state_t exec_timestep (state_t state, command_t *commands);

matrix_t exec_trace (trace_t trace, resolution_t resolution);

static inline energy_t
timestep_energy (state_t *state) {
    energy_t res = state->matrix.resolution;

    if (state->harmonics == High) {
        return 30 * res * res * res;
    } else if (state->harmonics == Low) {
        return 3 * res * res * res;
    } else {
        assert(false);
    }
}

static inline energy_t
bot_energy (state_t *state) {
    return 20 * state->n_bots;
}

static inline energy_t
smove_energy (coord_t lld) {
    return 2 * get_mlen(lld);
}

static inline energy_t
lmove_energy (coord_t sld1, coord_t sld2) {
    return 2 * (get_mlen(sld1) + 2 + get_mlen(sld2));
}

static inline energy_t
fill_energy (bool was_empty) {
    return was_empty ? 12 : 6;
}

static inline energy_t
fission_energy (void) {
    return 24;
}

static inline energy_t
fusion_energy (void) {
    return -24;
}
