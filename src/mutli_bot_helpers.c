#include "multi_bot_helpers.h"

bot_commands_t
internal_make_bot_commands(bot_t bot, int skipped_rounds) {
    bot_commands_t bc;
    bc.bot = bot;
    bc.cmds = g_array_sized_new (FALSE, FALSE, sizeof(command_t), skipped_rounds);
    command_t invalid;
    invalid.type = -1;
    for (int i = 0; i < skipped_rounds; i++) {
        g_array_append_val(bc.cmds, invalid);
    }
}

bot_commands_t make_bot_commands(bot_t bot) {
    return internal_make_bot_commands(bot, 0);
}

// returns the new bot
bot_commands_t fission(bot_commands_t* bc, coord_t rel_pos) {
    // when this asserts implement passing in m
    assert(bc->bot.n_seeds > 0);
    int m = bc->bot.n_seeds / 2;
    add_cmd(bc->cmds, fission_cmd(rel_pos, m));

    bot_t botp = make_bot(bc->bot.seeds[0], add_coords(bc->bot.pos, rel_pos), m, bc->bot.seeds + 1);
    bc->bot = make_bot(bc->bot.bid, bc->bot.pos, bc->bot.n_seeds - 1 - m, bc->bot.seeds + 1 + m);

    bot_commands_t new_bot = internal_make_bot_commands(botp, bc->cmds->len);

    return new_bot;
}

void fusion(bot_commands_t* bc_primary, bot_commands_t* bc_secondary) {
    
}