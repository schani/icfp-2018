#include "multi_bot_helpers.h"

#include "coord.h"

#include <glib.h>

#undef DEBUG_TRACE

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
    return bc;
}

bot_commands_t make_bot_commands(bot_t bot) {
    return internal_make_bot_commands(bot, 0);
}

multi_bot_commands_t make_multi_bot_commands(int n_bots) {
    multi_bot_commands_t mbc;
    mbc.n_bots = n_bots;
    mbc.bot_commands = calloc(sizeof(bot_commands_t), n_bots);
    return mbc;
}

// returns the new bot
bot_commands_t fission(bot_commands_t* bc, coord_t rel_pos) {
    return fission_with_m(bc, rel_pos, bc->bot.n_seeds/2);
}
bot_commands_t fission_with_m(bot_commands_t* bc, coord_t rel_pos, int m) {
    // when this asserts implement passing in m
    assert(bc->bot.n_seeds > 0);
    add_cmd(bc->cmds, fission_cmd(rel_pos, m));

    bot_t spawned = make_bot(bc->bot.seeds[0], add_coords(bc->bot.pos, rel_pos), m, bc->bot.seeds + 1);
    bc->bot = make_bot(bc->bot.bid, bc->bot.pos, bc->bot.n_seeds - 1 - m, bc->bot.seeds + 1 + m);

    bot_commands_t new_bot = internal_make_bot_commands(spawned, bc->cmds->len);

    return new_bot;
}

void fusion(bot_commands_t* bc_primary, bot_commands_t* bc_secondary) {
    coord_t nd_p = sub_coords(bc_secondary->bot.pos, bc_primary->bot.pos);
    coord_t nd_s = sub_coords(bc_primary->bot.pos, bc_secondary->bot.pos);

    assert(is_nd(nd_p));
    assert(is_nd(nd_s));
    
    add_cmd(bc_primary->cmds, fusionp_cmd(nd_p));
    add_cmd(bc_secondary->cmds, fusions_cmd(nd_s));

    // FIXME: if we ever fusion before fission: implement 
}

static int 
compare_bot_commands(const void *p1, const void *p2) {
    const bot_commands_t *bc1 = (const bot_commands_t*)p1;
    const bot_commands_t *bc2 = (const bot_commands_t*)p2;
    return compare_bots(&bc1->bot, &bc2->bot);
}

static inline void
sort_bot_commands(bot_commands_t* bot_commands, int n_bots) {
    return qsort(bot_commands, n_bots, sizeof(bot_commands_t), compare_bot_commands);
}

static multi_bot_commands_t
remove_bot(int fusioned, multi_bot_commands_t mbc) {
    // mbc is a copy ;-)
    mbc.n_bots = mbc.n_bots-1;
    mbc.bot_commands[fusioned] = mbc.bot_commands[mbc.n_bots-1];
    // FIXME bubble up rather thand sorting
    sort_bot_commands(mbc.bot_commands, mbc.n_bots);
    return mbc;
}

static inline 
command_t* mbc_get_command(multi_bot_commands_t mbc, int bot_index, int cmd_index) {
    return &g_array_index (mbc.bot_commands[bot_index].cmds, command_t, cmd_index);
}

static void
internal_merge_bot_commands(multi_bot_commands_t mbc, GArray* cmds, int cmd_offset) {
    sort_bot_commands(mbc.bot_commands, mbc.n_bots);

#ifdef DEBUG_TRACE
    printf("==== <merge> =====\n");
#endif  // DEBUG_TRACE

    while(mbc_get_command(mbc, 0, cmd_offset)->type != Halt) {
        for(int i=0; i<mbc.n_bots; i++) {
            command_t* curr = mbc_get_command(mbc, i, cmd_offset);
            if (curr->type >= 0 && curr->type <= COMMAND_MAX) {
                add_cmd(cmds, *curr);
#ifdef DEBUG_TRACE
                printf("%d %d:", i, mbc.bot_commands[i].bot.bid);
                print_cmd(*curr);
#endif  // DEBUG_TRACE
                if(curr->type == FusionS) {
                    mbc = remove_bot(i, mbc);
                    i--;
                }
            } else {
                // we skipped a prefix of invalid commands here.
            }
        }
        cmd_offset++;
#ifdef DEBUG_TRACE
        printf("==== %d =====\n", cmd_offset);
#endif  // DEBUG_TRACE
    }
    add_cmd(cmds, *mbc_get_command(mbc, 0, cmd_offset));

#ifdef DEBUG_TRACE
    print_cmd(*mbc_get_command(mbc, 0, cmd_offset));
    printf("==== </merge> =====\n");
#endif  // DEBUG_TRACE
}

GArray* merge_bot_commands(multi_bot_commands_t mbc) {
    int total_length = 0;
    for (int i=0; i<mbc.n_bots; i++) {
        total_length += mbc.bot_commands[i].cmds->len;
    }

    // overallocate because we will have some invalid at the beginning.
    GArray* cmds = g_array_sized_new (FALSE, FALSE, sizeof(command_t), total_length);
    internal_merge_bot_commands(mbc, cmds, 0);
    return cmds;
}

void equalize_some_multi_bot_commands(multi_bot_commands_t mbc, int first_n_bots) {
    assert(first_n_bots <= mbc.n_bots);
    int max_len = 0;
    for (int i = 0; i < first_n_bots; i++) {
        max_len = MAX(mbc.bot_commands[i].cmds->len, max_len);
    }
    command_t wait = wait_cmd();
    wait.coord1.x = 3;
    wait.coord1.y = 2;
    wait.coord1.z = 0;
    for (int i = 0; i < first_n_bots; i++) {
        while(mbc.bot_commands[i].cmds->len < max_len) {
            add_cmd(mbc.bot_commands[i].cmds, wait);
        }
    } 
}

void equalize_multi_bot_commands(multi_bot_commands_t mbc) {
     equalize_some_multi_bot_commands(mbc, mbc.n_bots);
}
