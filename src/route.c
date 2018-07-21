
#include <stdio.h>

#include "execution.h"
#include "route.h"


#define	MAX_COST	(250*3)



int	pick_pairs(int n, coord_t *pos, coord_t *end, int *sel, cost_func_t func)
{
	int cost, selp, sele;
	int fix[n];

	for (int i=0; i<n; i++) {
	   sel[i] = -1;
	   fix[i] = -1;
	}

	for (int k=0; k<n; k++) {

	    cost = MAX_COST;
	    selp = -1;
	    sele = -1;

	    // check minimal independent cost

	    for (int i=0; i<n; i++) {
		if (sel[i] >= 0)
		    continue;

		for (int j=0; j<n; j++) {
		    if (fix[j] >= 0)
			continue;

		    int new = func(&pos[i], &end[j]);

		    if (new < cost) {
			cost = new;
			selp = i;
			sele = j;
		    }
		}
	    }

	    sel[selp] = sele;
	    fix[sele] = selp;
	}
	return 0;
}



int	mark_move(matrix_t *m, coord_t *pos, coord_t *stop, const coord_t *dir, int cnt, int lim, voxel_t mark)
{
	coord_t curr = *pos;

	while (cnt) {
	    if (lim)
		lim--;
	    else {
		// printf("limit reached (%d left)\n", cnt);
		break;
	    }

	    coord_t new = {curr.x + dir->x, curr.y + dir->y, curr.z + dir->z};

	    if (get_voxel(m, new) == Empty) {
		set_voxel(m, new, mark);
		curr = new;
		cnt--;
	    } else {	// we hit something
		// printf("we hit something (%d) at <%d,%d,%d>\n", 
		//     get_voxel(m, new), new.x, new.y, new.z);
		break;
	    }
	}

	if (stop)
	    *stop = curr;

	// printf("*move <%d,%d,%d> -> <%d,%d,%d>\n",
	//     pos->x, pos->y, pos->z,
	//     curr.x, curr.y, curr.z);

	if (cnt) {
	    if (lim)
		return ERR_HIT;
	    return ERR_SHORT;
	}
	return SUCCESS;
}

int	mark_smove(matrix_t *m, coord_t *pos, coord_t *stop, const coord_t *dir, int cnt, voxel_t mark)
{
	result_t r = mark_move(m, pos, stop, dir, cnt, 15, mark);	

	// printf("smove <%d,%d,%d> -> <%d,%d,%d> [%d]\n",
	//     pos->x, pos->y, pos->z,
	//     stop->x, stop->y, stop->z, r);
	return r;
}

#define	test_smove(m, p, s, d, c) mark_smove(m, p, s, d, c, Empty)

int	mark_lmove(matrix_t *m, coord_t *pos, coord_t *mid, coord_t *stop, const coord_t *d1, int c1, coord_t *d2, int c2, voxel_t mark)
{
	result_t r1 = mark_move(m, pos, mid, d1, c1, 5, mark);	
	result_t r2 = mark_move(m, mid, stop, d2, c2, 5, mark);	
	
	// printf("lmove <%d,%d,%d> -> <%d,%d,%d> -> <%d,%d,%d> [%d,%d]\n",
	//     pos->x, pos->y, pos->z,
	//     mid->x, mid->y, mid->z,
	//     stop->x, stop->y, stop->z, r1, r2);
	   
	if ((r1 == ERR_HIT) || (r2 == ERR_HIT))
	    return ERR_HIT;
	if ((r1 == ERR_SHORT) || (r2 == ERR_SHORT))
	    return ERR_SHORT;
	return SUCCESS;
}

#define	test_lmove(m, p, i, s, d1, c1, d2, c2) mark_lmove(m, p, i, s, d1, c1, d2, c2, Empty)

typedef
enum {	
	Move_Unknown = 0,
	Move_SMove,
	Move_LMove,
} move_t;

int sign(int x) {
	return (x > 0) - (x < 0);
}

int	route_bot(matrix_t *m, coord_t *pos, coord_t *end, command_t *cmd)
{
	int dx = end->x - pos->x;	
	int dy = end->y - pos->y;	
	int dz = end->z - pos->z;	

	// printf("\nroute <%d,%d,%d> -> <%d,%d,%d> = [%d,%d,%d]\n",
	//     pos->x, pos->y, pos->z,
	//     end->x, end->y, end->z,
	//     dx, dy, dz);

	coord_t d1, d2;
	unsigned c1, c2;
	move_t type = Move_Unknown;

	if ((dx == 0) && (dy == 0)) {		// Straight dz
	    d1 = (coord_t){0, 0, sign(dz)};
	    c1 = abs(dz);
	    type = Move_SMove;

	} else if ((dx == 0) && (dz == 0)) {	// Straight dy
	    d1 = (coord_t){0, sign(dy), 0};
	    c1 = abs(dy);
	    type = Move_SMove;

	} else if ((dy == 0) && (dz == 0)) {	// Straight dx
	    d1 = (coord_t){sign(dx), 0, 0};
	    c1 = abs(dx);
	    type = Move_SMove;

	} else if (dx == 0) {			// LMove dy/dz
	    d1 = (coord_t){0, sign(dy), 0};
	    c1 = abs(dy);
	    d2 = (coord_t){0, 0, sign(dz)};
	    c2 = abs(dz);
	    type = Move_LMove;

	} else if (dy == 0) {			// LMove dx/dz
	    d1 = (coord_t){sign(dx), 0, 0};
	    c1 = abs(dx);
	    d2 = (coord_t){0, 0, sign(dz)};
	    c2 = abs(dz);
	    type = Move_LMove;

	} else if (dz == 0) {			// LMove dx/dy
	    d1 = (coord_t){sign(dx), 0, 0};
	    c1 = abs(dx);
	    d2 = (coord_t){0, sign(dy), 0};
	    c2 = abs(dy);
	    type = Move_LMove;

	} else {
	    d1 = (coord_t){0, sign(dy), 0};	// up/down
	    c1 = abs(dy);
	    type = Move_SMove;

	    // printf("3d move -> smove (dy)\n");
	}

	if (type == Move_SMove) {
	    coord_t stop;

	    result_t r = test_smove(m, pos, &stop, &d1, c1);
		
	    // printf("smove test r=%d\n", r);
	    if ((r == SUCCESS) || (r == ERR_SHORT))
		mark_smove(m, pos, &stop, &d1, c1, 42);
	    /* else
		evade(m, pos, end, cmd); */

	    if (r == ERR_HIT) {
		cmd->type = Wait;

		*end = *pos;
	    } else {
		cmd->type = SMove;
		cmd->SMove_lld = (coord_t)
		    {stop.x-pos->x, stop.y-pos->y, stop.z-pos->z};

		*end = stop;
	    }
	    return r;

	} else if (type == Move_LMove) {
	    coord_t mid, stop;

	    result_t r = test_lmove(m, pos, &mid, &stop, &d1, c1, &d2, c2);

	    // printf("lmove test r=%d\n", r);
	    if ((r == SUCCESS) || (r == ERR_SHORT))
		mark_lmove(m, pos, &mid, &stop, &d1, c1, &d2, c2, 42);
	    else {	// Try alternative
		r = test_lmove(m, pos, &mid, &stop, &d2, c2, &d1, c1);

		// printf("lmove test r=%d\n", r);
		if ((r == SUCCESS) || (r == ERR_SHORT))
		    mark_lmove(m, pos, &mid, &stop, &d1, c1, &d2, c2, 42);
		/* else
		    evade(m, pos, end, cmd); */
	    }

	    if (r == ERR_HIT) {
		cmd->type = Wait;

		*end = *pos;
	    } else {
		cmd->type = LMove;
		cmd->LMove_sld1 = (coord_t)
		    {mid.x-pos->x, mid.y-pos->y, mid.z-pos->z};
		cmd->LMove_sld2 = (coord_t)
		    {stop.x-mid.x, stop.y-mid.y, stop.z-mid.z};

		*end = stop;
	    }
	    return r;

	} else {  // What now?

	    cmd->type = Wait;
	    *end = *pos;
	    return ERR_LOST;
	}
}


int	route_bots(matrix_t *m, int n, coord_t *pos, coord_t *end, coord_t *stop, command_t *cmd, result_t *status)
{
	int sel[n];

	pick_pairs(n, pos, end, sel, cost_func);

	int fix[n];

	for (int i=0; i<n; i++) {
	   fix[i] = 0;
	}

	for (int k=0; k<n; k++) {

	    int cost = MAX_COST;
	    int pick = -1;

	    // select best candidate

	    for (int i=0; i<n; i++) {
		if (fix[i])
		    continue;

		int new = cost_func(&pos[i], &end[sel[i]]);

		// printf("[%d] -> [%d] = %d\n", i, sel[i], new);
		
		if (new < cost) {
		    cost = new;
		    pick = i;
		}
	    }

	    fix[pick] = 1;
	    stop[sel[pick]] = end[sel[pick]];
	    status[pick] = route_bot(m, &pos[pick], &stop[sel[pick]], &cmd[pick]);
	}
	return 0;
}



