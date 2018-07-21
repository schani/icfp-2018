#ifndef _ROUTE_H
#define	_ROUTE_H

#include "trace.h"
#include "model.h"

typedef int (*cost_func_t)(coord_t *, coord_t *);

int	route_bots(matrix_t *m, int n, coord_t *pos, coord_t *end, coord_t *stop, command_t *cmd, uint8_t *status);
int     pick_pairs(int n, coord_t *pos, coord_t *end, int *sel, cost_func_t func);


static inline
int	cost_func(coord_t *pos, coord_t *end)
{
	return abs(end->x - pos->x) + abs(end->y - pos->y) + abs(end->z - pos->z);
}

#endif	/* _ROUTE_H */
