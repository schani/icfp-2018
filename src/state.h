#include "trace.h"
#include "model.h"

typedef uint8_t bid_t;

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

typedef struct {
    int energy;
    harmonics_t harmonics;
    matrix_t matrix;
    int n_bots;
    bot_t *bots;
} state_t;

state_t make_state (void);

bot_t make_bot (bid_t bid, coord_t pos, int n_seeds, bid_t *seeds);
