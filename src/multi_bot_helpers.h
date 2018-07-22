#ifndef __MULTI_BOT_HELPERS_H
#define __MULTI_BOT_HELPERS_H

#include "coord.h"
#include "state.h"

#include <gmodule.h>

// contains invalid commands, do not execute without running through merge_bot_commands
typedef struct {
    bot_t bot;
    GArray* cmds; 
} bot_commands_t;

typedef struct {
    int n_bots;
    bot_commands_t* bot_commands; 
} multi_bot_commands_t;

multi_bot_commands_t make_multi_bot_commands(int n_bots);

bot_commands_t make_bot_commands(bot_t bot);
// returns the new bot
// FIXME add m parameter later if needed.
bot_commands_t fission(bot_commands_t* bc, coord_t rel_pos);
void fusion(bot_commands_t* bc_primary, bot_commands_t* bc_secondary);

static inline coord_t 
get_bot_pos(bot_commands_t* bc) { return bc->bot.pos; }

// FIXME: replace future usages of this by using execution.c code to modify the position. 
static inline
void set_bot_pos(bot_commands_t* bc, coord_t coord) {
    bc->bot.pos = coord;
}

// returns the command trace of all bots together 
GArray* merge_bot_commands(multi_bot_commands_t mbc);

#endif  // __MULTI_BOT_HELPERS_H