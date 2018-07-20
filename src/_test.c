/* exists to check all headers */

#include <stdio.h>
#include <glib.h>

#include "model.h"
#include "trace.h"

int
main() 
{
    GList *list = NULL;
    list = g_list_append( list, "muh (powered by glib)" );
    fprintf( stderr, "%s\n", g_list_first(list)->data );
    return 0;
}

