#ifndef _STATE_H
#define _STATE_H

#include "trace.h"
#include "model.h"

typedef uint8_t bid_t;

typedef enum {
    Lightning, 
    Thunderbolt, // because Full is taken
} task_mode_t;

typedef enum {
    Low,
    High
} harmonics_t;

typedef struct {
    bid_t bid;
    coord_t pos;
    int n_seeds;
    bid_t *seeds; // immutable!!!
} bot_t;

typedef int64_t energy_t;

typedef struct {
    energy_t energy;
    harmonics_t harmonics;
    matrix_t matrix;
    int n_bots;
    bot_t *bots;
} state_t;

state_t make_state (energy_t energy, harmonics_t harmonics, matrix_t matrix);
state_t make_lightning_start_state (resolution_t res);
state_t make_start_state_from_matrix(matrix_t matrix, task_mode_t mode);

bot_t make_bot (bid_t bid, coord_t pos, int n_seeds, bid_t *seeds);

int compare_bids (const void *p1, const void *p2);
int compare_bots (const void *p1, const void *p2);

#endif /* _STATE_H */