
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "nbtio.h"


int main(int argc, char *argv[])
{
	do {
	    int c = getchar();
	
	    if (c == EOF)
		break;

	    else if (c == 0xFF) {		// Halt
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
	    else if ((c & 0x7) == 0x2) {	// Void
		int nd = c >> 3;

		int dx = 0, dy = 0, dz = 0;
		int r = ndtoxyz(nd, &dx, &dy, &dz);
		
		if (r)
		    goto error_bad;

		printf("Void <%d,%d,%d>\n",
		    dx, dy, dz);

	    }
	    else if ((c & 0x7) == 0x1) {	// GFill
		int nd = c >> 3;

		int dx = 0, dy = 0, dz = 0;
		int r = ndtoxyz(nd, &dx, &dy, &dz);
		
		if (r)
		    goto error_bad;

		int fd[3] = {0};
		for (int i=0; i<3; i++) {
		    fd[i] = getchar();

		    if (fd[i] == EOF)
			goto error_short;
		}

		int fx = 0, fy = 0, fz = 0;
		r = fdtoxyz(fd, &fx, &fy, &fz);
		
		if (r)
		    goto error_bad;

		printf("GFill <%d,%d,%d> <%d,%d,%d>\n",
		    dx, dy, dz, fx, fy, fz);

	    }
	    else if ((c & 0x7) == 0x0) {	// GVoid
		int nd = c >> 3;

		int dx = 0, dy = 0, dz = 0;
		int r = ndtoxyz(nd, &dx, &dy, &dz);
		
		if (r)
		    goto error_bad;

		int fd[3] = {0};
		for (int i=0; i<3; i++) {
		    fd[i] = getchar();

		    if (fd[i] == EOF)
			goto error_short;
		}

		int fx = 0, fy = 0, fz = 0;
		r = fdtoxyz(fd, &fx, &fy, &fz);
		
		if (r)
		    goto error_bad;

		printf("GVoid <%d,%d,%d> <%d,%d,%d>\n",
		    dx, dy, dz, fx, fy, fz);

	    }
	    else {				// Unknown
		printf("Unknown!!!\n");
	    }
	} while (1);	
	exit(0);

error_short:
	exit(1);

error_bad:
	exit(2);

}

