#ifndef __MULTI_BOT_HELPERS_H
#define __MULTI_BOT_HELPERS_H

#include "coord.h"
#include "execution.h"

typedef struct{
    bot_t bot;
    GArray * cmds;
} bot_commands_t;

coord_t get_bot_pos(bot_commands_t* bc);
void set_bot_pos(bot_commands_t* bc, coord_t coord);

bot_commands_t fission(bot_commands_t* bc);
void fusion(bot_commands_t* bc_primary, bot_commands_t* bc_secondary);

#endif  // __MULTI_BOT_HELPERS_H