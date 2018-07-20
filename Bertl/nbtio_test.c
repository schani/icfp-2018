
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <gmodule.h>

#include "nbtio.h"


#define	ARRAY_SIZE	1024

GArray *read_nbt_array(FILE *f)
{
	GArray *a = g_array_sized_new(FALSE, FALSE, sizeof(command_t), ARRAY_SIZE);

	while (1) {
	    command_t cmd = { 0 };
	    int r = read_nbt_command(f, &cmd);   

	    if (r == ERR_EOF)
		break;
	    else if (r != SUCCESS) {
		g_array_free(a, TRUE);
		return NULL;
	    }

	    g_array_append_val(a, cmd); 
	}
	return a;
}

int write_nbt_array(FILE *f, GArray *a)
{
	for (int i=0; i<a->len; i++) {
	    command_t *cmd = &g_array_index(a, command_t, i);
	    
	    int r = write_nbt_command(f, cmd);
	    if (r != SUCCESS) {
		return r;
	    }
	}
	return SUCCESS;
}


int write_nbt_trace(FILE *f, trace_t *t)
{
	for (int i=0; i<t->n_timesteps; i++) {
	    timestep_t *s = &t->timesteps[i];

	    for (int j=0; j<s->n_commands; j++) {
		command_t *c = &s->commands[j];

		int r = write_nbt_command(f, c);
		if (r != SUCCESS) {
		    return r;
		}
	    }
	}
	return SUCCESS;
}



int main(int argc, char *argv[])
{
	GArray *a = read_nbt_array(stdin);

	if (a)
	    write_nbt_array(stdout, a);

	exit(0);

/*
	do {
	    command_t cmd = { 0 };
	    int r = read_nbt_command(stdin, &cmd);   

	    if (r == ERR_EOF)
		exit(0);

	    else if (r != SUCCESS) {
		fprintf(stderr, "read_error %d\n", r);
		exit(1);
	    }

	    r = write_nbt_command(stdout, &cmd);
	    if (r != SUCCESS) {
		fprintf(stderr, "write_error %d\n", r);
		exit(2);
	    }

	} while (1);
*/
}

