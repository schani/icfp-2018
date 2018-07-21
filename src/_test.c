/* exists to check all headers */

#include <stdio.h>
#include <glib.h>
#include <gc.h>

#include "model.h"
#include "trace.h"
#include "default_trace.h"

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
    set_voxel(&testModel, create_coord(3,0,3), Full);
    set_voxel(&testModel, create_coord(3,3,4), Full);
    /*set_voxel(&testModel, create_coord(3,1,1), Full);
    set_voxel(&testModel, create_coord(4,1,1), Full);
    set_voxel(&testModel, create_coord(5,1,1), Full);
    set_voxel(&testModel, create_coord(6,1,1), Full);
    set_voxel(&testModel, create_coord(7,1,1), Full);
    set_voxel(&testModel, create_coord(8,1,1), Full);
    set_voxel(&testModel, create_coord(9,1,1), Full);
    set_voxel(&testModel, create_coord(9,2,2), Full);
    */
    GArray *cmds = NULL;
    cmds = exec_default_trace(&testModel);
    
    
    int i;
    for (i=0; i<cmds->len; i++) {
        command_t cmd = (command_t)g_array_index(cmds, command_t, i);
        fprintf(stdout, "CMD#%i: %d (%i,%i,%i) (%i,%i,%i)\n", i, cmd.type, cmd.coord1.x, cmd.coord1.y, cmd.coord1.z, cmd.coord2.x, cmd.coord2.y, cmd.coord2.z);
    }



    /*
    typedef struct {
        int n_commands;
        command_t *commands;
    } timestep_t;

    typedef struct {
        int n_timesteps;
        timestep_t *timesteps;
    } trace_t;*/

    g_array_free (cmds, TRUE);

    return 0;
}

