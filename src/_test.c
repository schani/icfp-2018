/* exists to check all headers */

#include <stdio.h>
#include <glib.h>
#include <gc.h>

#include "model.h"
#include "trace.h"
#include "default_trace.h"




    void print_cmd(command_t * cmd){
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

int
main() 
{
    GC_INIT();

    /*GList *list = NULL;
    list = g_list_append( list, "muh (powered by glib)" );
    fprintf( stderr, "%s\n", g_list_first(list)->data );
    fprintf( stderr, "GC: Heap size = %zu\n", GC_get_heap_size());*/
    
    voxel_t tmp[10*10*10] = {0};
    matrix_t testModel;
    testModel.resolution = 10;
    testModel.data = tmp;
   
    set_voxel(&testModel, create_coord(2,0,1), Full);
    set_voxel(&testModel, create_coord(3,0,1), Full);
    set_voxel(&testModel, create_coord(3,1,1), Full);
    set_voxel(&testModel, create_coord(3,2,1), Full);
    set_voxel(&testModel, create_coord(3,3,1), Full);
    set_voxel(&testModel, create_coord(4,3,1), Full);
    set_voxel(&testModel, create_coord(4,2,1), Full);
    set_voxel(&testModel, create_coord(4,1,1), Full);
    set_voxel(&testModel, create_coord(4,0,1), Full);
    
    GArray *cmds = NULL;
    GArray *timesteps = g_array_new(FALSE, FALSE, sizeof(timestep_t));
    cmds = exec_default_trace(&testModel);
        
    int i;
    for (i=0; i<cmds->len; i++) {
        command_t cmd = (command_t)g_array_index(cmds, command_t, i);
<<<<<<< HEAD
        print_cmd(&cmd);
        //fprintf(stdout, "CMD#%i: %d (%i,%i,%i) (%i,%i,%i)\n", i, cmd.type, cmd.coord1.x, cmd.coord1.y, cmd.coord1.z, cmd.coord2.x, cmd.coord2.y, cmd.coord2.z);
    }
=======
        fprintf(stdout, "CMD#%i: %d (%i,%i,%i) (%i,%i,%i)\n", i, cmd.type, cmd.coord1.x, cmd.coord1.y, cmd.coord1.z, cmd.coord2.x, cmd.coord2.y, cmd.coord2.z);
>>>>>>> 4319efab19c728d57d08b3be83c327ac6d90057b

        timestep_t tstmp;
        tstmp.n_commands = 1;
        tstmp.commands = &cmd;
        g_array_append_val(timesteps, tstmp);
    }

<<<<<<< HEAD



    /*
    typedef struct {
        int n_commands;
        command_t *commands;
    } timestep_t;

    typedef struct {
        int n_timesteps;
        timestep_t *timesteps;
    } trace_t;*/
=======
    trace_t trace;
    trace.timesteps = (timestep_t*)timesteps->data;
    trace.n_timesteps = timesteps->len;
>>>>>>> 4319efab19c728d57d08b3be83c327ac6d90057b

    g_array_free (cmds, TRUE);
    g_array_free (timesteps, TRUE);

    return 0;
}

