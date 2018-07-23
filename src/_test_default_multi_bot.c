#include <stdio.h>

#include "model.h"
#include "default_trace.h"
#include "commands.h"
#include "state.h"
#include "strategy_flush_at_once.h"
#include "default_multi_bots.h"


int main(int argc, char** argv) {
    assert(argc == 2);
  
    matrix_t m = read_model_file(argv[1]);

    state_t start = make_start_state_from_matrix(m, Thunderbolt);

    GArray* cmds =  calc_multi_bot_default_trace(&m, start.bots[0]);

    for (int i=0; i<cmds->len; i++) {
        command_t cmd = (command_t)g_array_index(cmds, command_t, i);
        print_cmd(cmd);
    }
    return 0;
}