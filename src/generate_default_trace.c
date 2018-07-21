#include "model.h"
#include "default_trace.h"

#include <stdio.h>



    static void print_cmd(command_t * cmd){
        switch (cmd->type) {
        case Halt:
            printf("Halt\n");
            break;

        case Wait:
            printf("Wait\n");
            break;

        case Flip:
            printf("Flip\n");
            break;

        case SMove: {
            printf("SMove   %d %d %d\n", cmd->SMove_lld.x, cmd->SMove_lld.y, cmd->SMove_lld.z);
            break;
        }

        case LMove: {
            printf("LMove   %d %d %d   %d %d %d\n", cmd->LMove_sld1.x, cmd->LMove_sld1.y, cmd->LMove_sld1.z, cmd->LMove_sld2.x, cmd->LMove_sld2.y, cmd->LMove_sld2.z);
            break;
        }

        case Fission: {
            printf("Fission %d %d %d  %d\n", cmd->Fission_nd.x, cmd->Fission_nd.y, cmd->Fission_nd.z, cmd->m);
            break;
        }

        case Fill: {
            printf("Fill    %d %d %d\n", cmd->Fill_nd.x, cmd->Fill_nd.y, cmd->Fill_nd.z);
            break;
        }

        case FusionP: {
            printf("FusionP %d %d %d\n", cmd->FusionP_nd.x, cmd->FusionP_nd.y, cmd->FusionP_nd.z);
            break;
        }

        case FusionS: {
            printf("FusionS %d %d %d\n", cmd->FusionS_nd.x, cmd->FusionS_nd.y, cmd->FusionS_nd.z);
            break;
        }

        default:
            assert(false);
        }

    }


int main(int argc, char** argv) {
    if (argc != 2) {
        assert(false);
    }
    matrix_t m = read_model_file(argv[1]);
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
        
    GArray* cmds = exec_default_trace(&m);

    int i;
    for (i=0; i<cmds->len; i++) {
        command_t cmd = (command_t)g_array_index(cmds, command_t, i);
        print_cmd(&cmd);
        //fprintf(stdout, "CMD#%i: %d (%i,%i,%i) (%i,%i,%i)\n", i, cmd.type, cmd.coord1.x, cmd.coord1.y, cmd.coord1.z, cmd.coord2.x, cmd.coord2.y, cmd.coord2.z);
    }
    g_array_free (cmds, TRUE);

    return 0;
}