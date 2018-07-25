#ifndef __DEFAULT_MULTI_BOT
#define __DEFAULT_MULTI_BOT

#include <gmodule.h>
#include "multi_bot_helpers.h"
#include "state.h"

GArray* calc_multi_bot_default_trace(matrix_t *m, bot_t initial_bot);

void calc_multi_bot_constructor_trace(matrix_t *m, multi_bot_commands_t *mbc);

void gather_grid_bots(multi_bot_commands_t* mbc, int safe_height, int first_bot);

#endif // __DEFAULT_MULTI_BOT
