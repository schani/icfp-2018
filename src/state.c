#include <string.h>

#include "state.h"

state_t
make_state (void) {
    state_t s;
    memset(&s, 0, sizeof(state_t));
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
