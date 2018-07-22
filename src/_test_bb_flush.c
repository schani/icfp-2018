#include <stdio.h>

#include "model.h"
#include "default_trace.h"
#include "commands.h"
#include "state.h"
#include "strategy_flush_at_once.h"

extern GArray* exec_test_bb_flush(matrix_t *mdl, bot_t *bot1);

int main(int argc, char** argv) {

  
    if (argc != 2) {
        assert(argc == 2);
    }
    matrix_t m = read_model_file(argv[1]);

    /* 
    int r = m.resolution - 1;

    coord_t origin = create_coord(0,0,0);
    region_t x_0 = make_region(origin, create_coord(0,r,r));
    assert(region_is_empty(&m, x_0));
    region_t x_r = make_region(create_coord(r,0,0), create_coord(r,r,r));
    assert(region_is_empty(&m, x_r));
    region_t z_0 = make_region(origin, create_coord(r,r,0));
    assert(region_is_empty(&m, z_0));
    region_t z_r = make_region(create_coord(0,0,r), create_coord(r,r,r));
    assert(region_is_empty(&m, z_r));
    region_t y_r = make_region(create_coord(0,r,0), create_coord(r,r,r));
    assert(region_is_empty(&m, y_r));

    */
    state_t start = make_start_state_from_matrix(m, Thunderbolt);



    GArray* cmds = exec_test_bb_flush(&start.matrix, &start.bots[0]);

    int i;
    for (i=0; i<cmds->len; i++) {
        command_t cmd = (command_t)g_array_index(cmds, command_t, i);
        print_cmd(cmd);
        //fprintf(stdout, "CMD#%i: %d (%i,%i,%i) (%i,%i,%i)\n", i, cmd.type, cmd.coord1.x, cmd.coord1.y, cmd.coord1.z, cmd.coord2.x, cmd.coord2.y, cmd.coord2.z);
    }
    g_array_free (cmds, TRUE);

    return 0;
}