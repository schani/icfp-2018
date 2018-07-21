
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <gmodule.h>

#include "nbtio.h"


int main(int argc, char *argv[])
{
	trace_t t;
	int err = read_trace(stdin, &t);

	if (err != SUCCESS) {
		return 1;
	}

	write_trace(stdout, t);

	return 0;

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

