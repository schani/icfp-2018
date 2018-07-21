#include <string.h>

#include "state.h"

state_t
make_state (energy_t energy, harmonics_t harmonics, matrix_t matrix) {
    state_t s;
    s.energy = energy;
    s.harmonics = harmonics;
    s.matrix = matrix;

    s.n_bots = 0;
    s.bots = NULL;

    return s;
}

bot_t
make_bot (bid_t bid, coord_t pos, int n_seeds, bid_t *seeds) {
    bot_t bot;
    bot.bid = bid;
    bot.pos = pos;
    bot.n_seeds = n_seeds;
    if (seeds == NULL) {
        bot.seeds = calloc(sizeof(bid_t), n_seeds);
    } else {
        bot.seeds = seeds;
    }
    return bot;
}
