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

    GList *list = NULL;
    list = g_list_append( list, "muh (powered by glib)" );
    fprintf( stderr, "%s\n", g_list_first(list)->data );

    fprintf( stderr, "GC: Heap size = %zu\n", GC_get_heap_size());

    return 0;
}

