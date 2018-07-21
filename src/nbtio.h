
#include "trace.h"

int read_nbt_command(FILE *, command_t *);
int write_nbt_command(FILE *, command_t *);

int read_trace(FILE *f, trace_t *t);
int write_trace(FILE *f, trace_t t);

typedef enum {
    SUCCESS = 0,
    ERR_EOF,
    ERR_BAD,
    ERR_SHORT,
    ERR_RANGE,
} nbtio_err_t;


static inline
int lldtoai(int dx, int dy, int dz, int *a, int *i)
{
	if (dx && (dy == 0) && (dz == 0)) {
	    *a = 1;

	    if ((dx > -15) && (dx < 15))
		*i = dx + 15;
	    else
		return -2;
	} else if (dy && (dx == 0) && (dz == 0)) {
	    *a = 2;

	    if ((dy > -15) && (dy < 15))
		*i = dy + 15;
	    else
		return -2;
	} else if (dz && (dx == 0) && (dy == 0)) {
	    *a = 3;

	    if ((dz > -15) && (dz < 15))
		*i = dz + 15;
	    else
		return -2;
	} else 
	    return -1;
	return 0;
}

static inline
int sldtoai(int dx, int dy, int dz, int *a, int *i)
{
	if (dx && (dy == 0) && (dz == 0)) {
	    *a = 1;

	    if ((dx > -5) && (dx < 5))
		*i = dx + 5;
	    else
		return -2;
	} else if (dy && (dx == 0) && (dz == 0)) {
	    *a = 2;

	    if ((dy > -5) && (dy < 5))
		*i = dy + 5;
	    else
		return -2;
	} else if (dz && (dx == 0) && (dy == 0)) {
	    *a = 3;

	    if ((dz > -5) && (dz < 5))
		*i = dz + 5;
	    else
		return -2;
	} else 
	    return -1;
	return 0;
}

static inline
int xyztond(int dx, int dy, int dz, int *nd)
{
	if ((dx < -1) || (dx > 1))
	    return -1;
	if ((dy < -1) || (dy > 1))
	    return -1;
	if ((dz < -1) || (dz > 1))
	    return -1;

	*nd = (dx + 1)*9 + (dy + 1)*3 + (dz + 1);

	// check for illegal combinations

	return 0;
}

static inline
int aitolld(int a, int i, int *dx, int *dy, int *dz)
{
	if (i > 30)		// -15 ... 15
	    return -2;

	if (a == 1)
	    *dx = i - 15;
	else if (a == 2)
	    *dy = i - 15;
	else if (a == 3)
	    *dz = i - 15;
	else
	    return -1;
	return 0;
}

static inline
int aitosld(int a, int i, int *dx, int *dy, int *dz)
{
	if (i > 10)		// -5 ... 5
	    return -2;

	if (a == 1)
	    *dx = i - 5;
	else if (a == 2)
	    *dy = i - 5;
	else if (a == 3)
	    *dz = i - 5;
	else
	    return -1;
	return 0;
}

static inline
int ndtoxyz(int nd, int *dx, int *dy, int *dz)
{
	int rz = nd % 3;
	int ry = (nd / 3) % 3;
	int rx = (nd / 9) % 3;

	*dx = rx - 1;
	*dy = ry - 1;
	*dz = rz - 1;

	// check for illegal values

	return 0;
}


