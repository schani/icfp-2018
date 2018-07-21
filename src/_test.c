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
    set_voxel(&testModel, create_coord(3,0,1), Full);
    set_voxel(&testModel, create_coord(3,1,1), Full);
    set_voxel(&testModel, create_coord(3,2,1), Full);
    set_voxel(&testModel, create_coord(3,3,1), Full);
    set_voxel(&testModel, create_coord(4,3,1), Full);
    set_voxel(&testModel, create_coord(4,2,1), Full);
    set_voxel(&testModel, create_coord(4,1,1), Full);
    set_voxel(&testModel, create_coord(4,0,1), Full);
    
    GArray *cmds = exec_default_trace(&testModel);
        
    int i;
    for (i=0; i<cmds->len; i++) {
        command_t cmd = (command_t)g_array_index(cmds, command_t, i);
        print_cmd(cmd);
    
    }

    trace_t trace;
    trace.n_commands = cmds->len;
    trace.commands = (command_t*)g_array_free (cmds, FALSE);

    return 0;
}

