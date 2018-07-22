#include <string.h>

#include "state.h"

int
compare_bids (const void *p1, const void *p2) {
    bid_t b1 = *(const bid_t*)p1;
    bid_t b2 = *(const bid_t*)p2;
    if (b1 < b2) return -1;
    if (b1 == b2) return 0;
    return 1;
}

int
compare_bots (const void *p1, const void *p2) {
    const bot_t *b1 = (const bot_t*)p1;
    const bot_t *b2 = (const bot_t*)p2;
    return compare_bids(&b1->bid, &b2->bid);
}

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

state_t
make_start_state (resolution_t res) {
    state_t state = make_state(0, Low, make_matrix(res));
    bid_t *seeds = malloc(sizeof(bid_t) * 19);
    for (int i = 0; i < 19; i++) {
        seeds[i] = i + 2;
    }
    state.n_bots = 1;
    state.bots = malloc(sizeof(bot_t) * 1);
    state.bots[0] = make_bot(1, create_coord(0, 0, 0), 19, seeds);
    return state;
}
