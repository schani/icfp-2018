

#include <stdio.h>

#include "route.h"

int main(int argc, char *argv[])
{
	matrix_t m = make_matrix(20);

	coord_t pos[] = {{0,0,0}, {1,0,0}, {0,0,1}};	
	coord_t end[] = {{1,3,0}, {0,16,1}, {2,3,5}};	
	coord_t stop[3];

	command_t cmd[3];
	result_t status[3];


	coord_t block = {0,2,0};
	set_voxel(&m, block, Full);

	route_bots(&m, 3, pos, end, stop, cmd, status);

	exit(0);
}
