
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


static inline
int is_white(int c)
{
	if ((c == ' ') || (c == '\t'))
	    return 1;
	return 0;
}

static inline
int is_ignore(int c)
{
	if ((c == '<') || (c == '[') || (c == '(') ||
	    (c == '>') || (c == ']') || (c == ')') ||
	    (c == ','))
	    return 1;
	return 0;
}


static inline
int is_eol(int c)
{
	if ((c == '\n') || (c == '\r'))
	    return 1;
	return 0;
}

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


#define COMP(s, c, l) (!comp_len(s, c, sizeof(s)-1, l))

int 	comp_len(const char *a, const char *b, unsigned la, unsigned lb)
{
	fprintf(stderr, ">%*.*s< >%*.*s< [%d,%d]\n", la, la, a, lb, lb, b, la, lb);
	if (la != lb)
	    return -1;
	if (strncmp(a, b, la))
	    return -2;
	return 0;
}


#define	CMD_MAX	8
#define	ARG_MAX 6


int main(int argc, char *argv[])
{
	enum {
	    s_start,
	    s_cmd,
	    s_arg,
	    s_num,
	    s_skip,
	} state = s_start;
		
	int ni = 0;
	int num[ARG_MAX];
	int sign = 0;

	int ci = 0;
	char cmd[CMD_MAX];

	int c = 0;
	unsigned line = 0;
	unsigned col = 0;

	do {
	    do {
		c = getchar();
		col++;
	    
		if (c == EOF)
		    break;

		if ((c >= 'a') && (c <= 'z'))
		    c = c - 'a' + 'A';
		
		switch (state) {
		case s_start:
		    if (is_white(c) || is_eol(c)) {
			continue;
		    } 
		    else if ((c >= 'A') && (c <= 'Z')) {
			if (ci < CMD_MAX) {
			    cmd[ci++] = c;
			    state = s_cmd;
			}
			else
			    goto error_ocmd;
		    }
		    else if ((c == '#') || (c == ';')) {
			state = s_skip;
		    }
		    else 
			goto error_bad;
		    break;

		case s_cmd:
		    if (is_white(c) || is_ignore(c) || is_eol(c)) {
			state = s_arg;
			continue;
		    } 
		    else if ((c >= 'A') && (c <= 'Z')) {
			if (ci < CMD_MAX) {
			    cmd[ci++] = c;
			}
			else
			    goto error_ocmd;
		    }
		    else 
			goto error_bad;
		    break;

		case s_arg:
		    if (is_white(c) || is_ignore(c) || is_eol(c)) {
			continue;
		    } 
		    else if ((c >= '0') && (c <= '9')) {
			if (ni < ARG_MAX) {
			    num[ni] = c - '0';
			    sign = 1;
			    state = s_num;
			}
			else
			    goto error_oarg;
		    }
		    else if (c == '-') {
			if (ni < ARG_MAX) {
			    num[ni] = 0;
			    sign = -1;
			    state = s_num;
			}
			else
			    goto error_oarg;
		    }
		    else if ((c == '#') || (c == ';')) {
			state = s_skip;
		    }
		    else 
			goto error_bad;
		    break;

		case s_num:
		    if (is_white(c) || is_ignore(c) || is_eol(c)) {
			num[ni++] *= sign;
			state = s_arg;
		    } 
		    else if ((c >= '0') && (c <= '9')) {
			num[ni] = (num[ni] * 10) + (c - '0');
		    }
		    else 
			goto error_bad;
		    break;

		case s_skip:
		    break;

		}

	    } while (!is_eol(c));

	    if (c == EOF)
		break;

	    fprintf(stderr, ">%*.*s<\t", ci, ci, cmd);
	    for (int i=0; i<ni; i++)
		fprintf(stderr, "[%d]\t", num[i]);
	    fprintf(stderr, "\n");

	    if (COMP("HALT", cmd, ci)) {
		if (ni)
		    goto error_extra;

		putchar(0xFF);
	    }
	    else if (COMP("WAIT", cmd, ci)) {
		if (ni)
		    goto error_extra;

		putchar(0xFE);
	    }
	    else if (COMP("FLIP", cmd, ci)) {
		if (ni)
		    goto error_extra;

		putchar(0xFD);
	    }
	    else if (COMP("SMOVE", cmd, ci)) {
		if (ni > 3)
		    goto error_extra;
		else if (ni < 3)
		    goto error_miss;

		int a = 0, i = 0;
		int r = lldtoai(num[0], num[1], num[2], &a, &i);

		if (r)
		    goto error_value;

		putchar(0x04 | (a << 4));
		putchar(i);
	    }
	    else if (COMP("LMOVE", cmd, ci)) {
		if (ni > 6)
		    goto error_extra;
		else if (ni < 6)
		    goto error_miss;

		int a1 = 0, i1 = 0;
		int r = sldtoai(num[0], num[1], num[2], &a1, &i1);

		if (r)
		    goto error_value;

		int a2 = 0, i2 = 0;
		r = sldtoai(num[3], num[4], num[5], &a2, &i2);

		if (r)
		    goto error_value;

		putchar(0x0C | (a1 << 4) | (a2 << 6));
		putchar(i1 | (i2 << 4));
	    }
	    else if (COMP("FUSIONP", cmd, ci)) {
		if (ni > 3)
		    goto error_extra;
		else if (ni < 3)
		    goto error_miss;

		int nd = 0;
		int r = xyztond(num[0], num[1], num[2], &nd);

		if (r)
		    goto error_value;
	
		putchar(0x07 | (nd << 3));
	    }
	    else if (COMP("FUSIONS", cmd, ci)) {
		if (ni > 3)
		    goto error_extra;
		else if (ni < 3)
		    goto error_miss;

		int nd = 0;
		int r = xyztond(num[0], num[1], num[2], &nd);

		if (r)
		    goto error_value;
	
		putchar(0x06 | (nd << 3));
	    }
	    else if (COMP("FISSION", cmd, ci)) {
		if (ni > 4)
		    goto error_extra;
		else if (ni < 4)
		    goto error_miss;

		int nd = 0;
		int r = xyztond(num[0], num[1], num[2], &nd);

		if (r)
		    goto error_value;
	
		putchar(0x05 | (nd << 3));
		putchar(num[3]);
	    }
	    else if (COMP("FILL", cmd, ci)) {
		if (ni > 3)
		    goto error_extra;
		else if (ni < 3)
		    goto error_miss;

		int nd = 0;
		int r = xyztond(num[0], num[1], num[2], &nd);

		if (r)
		    goto error_value;
	
		putchar(0x03 | (nd << 3));
	    }
	    else {
		goto error_cmd;
	    }

	    ni = 0;
	    ci = 0;
	    sign = 0;
	    col = 0;
	    line++;
	    state = s_start;
	} while (1);

	exit(0);


/*

	    else if (c == '-') {		// Neg
		neg = 1;
	    }
	    else if (c >= '0') {		// Halt
		printf("Halt\n");
	    }
	    else if (c == 0xFE) {		// Wait
		printf("Wait\n");
	    }
	    else if (c == 0xFD) {		// Flip
		printf("Flip\n");
	    }
	    else if ((c & 0xF) == 0x4) {	// SMove
		int a = (c >> 4) & 0x3;

		if (c & 0xC0)
		    goto error_bad;

		int d = getchar();

		if (d == EOF)
		    goto error_short;

		int i = d & 0x3F;

		if (d != i)
		    goto error_bad;

		int dx = 0, dy = 0, dz = 0;
		int r = aitolld(a, i, &dx, &dy, &dz);

		if (r)
		    goto error_bad;

		printf("SMove <%d,%d,%d>\n",
		    dx, dy, dz);

	    }
	    else if ((c & 0xF) == 0xC) {	// LMove
		int a1 = (c >> 4) & 0x3;
		int a2 = (c >> 6) & 0x3;
		int d = getchar();

		if (d == EOF)
		    goto error_short;

		int i1 = (d >> 0) & 0xF;
		int i2 = (d >> 4) & 0xF;

		int d1x = 0, d1y = 0, d1z = 0;
		int d2x = 0, d2y = 0, d2z = 0;

		int r = aitosld(a1, i1, &d1x, &d1y, &d1z);

		if (r)
		    goto error_bad;

		r = aitosld(a2, i2, &d2x, &d2y, &d2z);

		if (r)
		    goto error_bad;

		printf("LMove <%d,%d,%d> <%d,%d,%d>\n",
		    d1x, d1y, d1z, d2x, d2y, d2z);
	    }
	    else if ((c & 0x7) == 0x7) {	// FusionP
		int nd = c >> 3;

		int dx = 0, dy = 0, dz = 0;
		int r = ndtoxyz(nd, &dx, &dy, &dz);

		if (r)
		    goto error_bad;

		printf("FusionP <%d,%d,%d>\n",
		    dx, dy, dz);
	    }
	    else if ((c & 0x7) == 0x6) {	// FusionS
		int nd = c >> 3;

		int dx = 0, dy = 0, dz = 0;
		int r = ndtoxyz(nd, &dx, &dy, &dz);

		if (r)
		    goto error_bad;

		printf("FusionS <%d,%d,%d>\n",
		    dx, dy, dz);
	    }
	    else if ((c & 0x7) == 0x5) {	// Fission
		int nd = c >> 3;
		int m = getchar();

		if (m == EOF)
		    goto error_short;

		int dx = 0, dy = 0, dz = 0;
		int r = ndtoxyz(nd, &dx, &dy, &dz);
		
		if (r)
		    goto error_bad;

		printf("Fission <%d,%d,%d> %d\n",
		    dx, dy, dz, m);

	    }
	    else if ((c & 0x7) == 0x3) {	// Fill
		int nd = c >> 3;

		int dx = 0, dy = 0, dz = 0;
		int r = ndtoxyz(nd, &dx, &dy, &dz);
		
		if (r)
		    goto error_bad;

		printf("Fill <%d,%d,%d>\n",
		    dx, dy, dz);
	    }
	    else {				// Unknown
		printf("Unknown!!!\n");
	    }
	} while (1);	
	exit(0);
*/

error_bad:
	fprintf(stderr, "bad character (%c) @ %d:%d\n", c, line, col);
	exit(1);

error_ocmd:
	fprintf(stderr, "command buffer overflow @ %d:%d\n", line, col);
	exit(2);

error_oarg:
	fprintf(stderr, "argument buffer overflow @ %d:%d\n", line, col);
	exit(3);

error_extra:
	fprintf(stderr, "extra arguments @ %d\n", line);
	exit(4);

error_miss:
	fprintf(stderr, "missing arguments @ %d\n", line);
	exit(5);

error_cmd:
	fprintf(stderr, "unknown command @ %d\n", line);
	exit(6);

error_value:
	fprintf(stderr, "bad value @ %d\n", line);
	exit(6);
}

