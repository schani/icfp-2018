
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <gmodule.h>

#include "nbtio.h"


int read_nbt_command(FILE *f, command_t *cmd)
{
	int c = fgetc(f);
    
	if (c == EOF)
	    return ERR_EOF;

	else if (c == 0xFF) {		// Halt
	    cmd->type = Halt;
	}
	else if (c == 0xFE) {		// Wait
	    cmd->type = Wait;
	}
	else if (c == 0xFD) {		// Flip
	    cmd->type = Flip;
	}
	else if ((c & 0xF) == 0x4) {	// SMove
	    int a = (c >> 4) & 0x3;

	    if (c & 0xC0)
		return ERR_BAD;

	    int d = fgetc(f);

	    if (d == EOF)
		return ERR_SHORT;

	    int i = d & 0x3F;

	    if (d != i)
		return ERR_BAD;

	    int dx = 0, dy = 0, dz = 0;
	    int r = aitolld(a, i, &dx, &dy, &dz);

	    if (r)
		return ERR_RANGE;

	    cmd->type = SMove;
	    cmd->SMove_lld.x = dx;
	    cmd->SMove_lld.y = dy;
	    cmd->SMove_lld.z = dz;
	}
	else if ((c & 0xF) == 0xC) {	// LMove
	    int a1 = (c >> 4) & 0x3;
	    int a2 = (c >> 6) & 0x3;
	    int d = fgetc(f);

	    if (d == EOF)
		return ERR_SHORT;

	    int i1 = (d >> 0) & 0xF;
	    int i2 = (d >> 4) & 0xF;

	    int d1x = 0, d1y = 0, d1z = 0;
	    int d2x = 0, d2y = 0, d2z = 0;

	    int r = aitosld(a1, i1, &d1x, &d1y, &d1z);

	    if (r)
		return ERR_RANGE;

	    r = aitosld(a2, i2, &d2x, &d2y, &d2z);

	    if (r)
		return ERR_RANGE;

	    cmd->type = LMove;
	    cmd->LMove_sld1.x = d1x;
	    cmd->LMove_sld1.y = d1y;
	    cmd->LMove_sld1.z = d1z;
	    cmd->LMove_sld2.x = d2x;
	    cmd->LMove_sld2.y = d2y;
	    cmd->LMove_sld2.z = d2z;
	}
	else if ((c & 0x7) == 0x7) {	// FusionP
	    int nd = c >> 3;

	    int dx = 0, dy = 0, dz = 0;
	    int r = ndtoxyz(nd, &dx, &dy, &dz);

	    if (r)
		return ERR_RANGE;	

	    cmd->type = FusionP;
	    cmd->FusionP_nd.x = dx;
	    cmd->FusionP_nd.y = dy;
	    cmd->FusionP_nd.z = dz;
	}
	else if ((c & 0x7) == 0x6) {	// FusionS
	    int nd = c >> 3;

	    int dx = 0, dy = 0, dz = 0;
	    int r = ndtoxyz(nd, &dx, &dy, &dz);

	    if (r)
		return ERR_RANGE;	

	    cmd->type = FusionS;
	    cmd->FusionS_nd.x = dx;
	    cmd->FusionS_nd.y = dy;
	    cmd->FusionS_nd.z = dz;
	}
	else if ((c & 0x7) == 0x5) {	// Fission
	    int nd = c >> 3;
	    int m = fgetc(f);

	    if (m == EOF)
		return ERR_SHORT;	

	    int dx = 0, dy = 0, dz = 0;
	    int r = ndtoxyz(nd, &dx, &dy, &dz);
	    
	    if (r)
		return ERR_RANGE;	

	    cmd->type = Fission;
	    cmd->Fission_nd.x = dx;
	    cmd->Fission_nd.y = dy;
	    cmd->Fission_nd.z = dz;
	    cmd->m = m;
	}
	else if ((c & 0x7) == 0x3) {	// Fill
	    int nd = c >> 3;

	    int dx = 0, dy = 0, dz = 0;
	    int r = ndtoxyz(nd, &dx, &dy, &dz);
	    
	    if (r)
		return ERR_RANGE;	

	    cmd->type = Fill;
	    cmd->Fill_nd.x = dx;
	    cmd->Fill_nd.y = dy;
	    cmd->Fill_nd.z = dz;
	}
	else if ((c & 0x7) == 0x2) {	// Void
	    int nd = c >> 3;

	    int dx = 0, dy = 0, dz = 0;
	    int r = ndtoxyz(nd, &dx, &dy, &dz);
	    
	    if (r)
		return ERR_RANGE;	

	    cmd->type = Void;
	    cmd->Void_nd.x = dx;
	    cmd->Void_nd.y = dy;
	    cmd->Void_nd.z = dz;
	}
	else if ((c & 0x7) == 0x1) {	// GFill
	    int nd = c >> 3;

	    int dx = 0, dy = 0, dz = 0;
	    int r = ndtoxyz(nd, &dx, &dy, &dz);
	    
	    if (r)
		return ERR_RANGE;	

	    int fd[3] = {0};
	    for (int i=0; i<3; i++) {
		fd[i] = fgetc(f);

		if (fd[i] == EOF)
		    return ERR_SHORT;
	    }

	    int fx = 0, fy = 0, fz = 0;
	    r = fdtoxyz(fd, &fx, &fy, &fz);
	    
	    if (r)
		return ERR_RANGE;

	    cmd->type = GFill;
	    cmd->GFill_nd.x = dx;
	    cmd->GFill_nd.y = dy;
	    cmd->GFill_nd.z = dz;
	    cmd->GFill_fd.x = fx;
	    cmd->GFill_fd.y = fy;
	    cmd->GFill_fd.z = fz;
	}
	else if ((c & 0x7) == 0x0) {	// GVoid
	    int nd = c >> 3;

	    int dx = 0, dy = 0, dz = 0;
	    int r = ndtoxyz(nd, &dx, &dy, &dz);
	    
	    if (r)
		return ERR_RANGE;	

	    int fd[3] = {0};
	    for (int i=0; i<3; i++) {
		fd[i] = fgetc(f);

		if (fd[i] == EOF)
		    return ERR_SHORT;
	    }

	    int fx = 0, fy = 0, fz = 0;
	    r = fdtoxyz(fd, &fx, &fy, &fz);
	    
	    if (r)
		return ERR_RANGE;

	    cmd->type = GVoid;
	    cmd->GVoid_nd.x = dx;
	    cmd->GVoid_nd.y = dy;
	    cmd->GVoid_nd.z = dz;
	    cmd->GVoid_fd.x = fx;
	    cmd->GVoid_fd.y = fy;
	    cmd->GVoid_fd.z = fz;
	}
	else {				// Unknown
	    return ERR_BAD;
	}
	return SUCCESS;
}

int write_nbt_command(FILE *f, command_t *cmd)
{
    
	switch (cmd->type) {
	case Halt: {
	    putc(0xFF, f);
	    break; }

	case Wait: {
	    putc(0xFE, f);
	    break; }

	case Flip: {
	    putc(0xFD, f);
	    break; }

	case SMove: {
	    int a = 0, i = 0;
	    int r = lldtoai(cmd->SMove_lld.x, cmd->SMove_lld.y, cmd->SMove_lld.z, &a, &i);

	    if (r)
		return ERR_RANGE;

	    putc(0x04 | (a << 4), f);
	    putc(i, f);
	    break; }

	case LMove: {
	    int a1 = 0, i1 = 0;
	    int r = sldtoai(cmd->LMove_sld1.x, cmd->LMove_sld1.y, cmd->LMove_sld1.z, &a1, &i1);

	    if (r)
		return ERR_RANGE;

	    int a2 = 0, i2 = 0;
	    r = sldtoai(cmd->LMove_sld2.x, cmd->LMove_sld2.y, cmd->LMove_sld2.z, &a2, &i2);

	    if (r)
		return ERR_RANGE;

	    putc(0x0C | (a1 << 4) | (a2 << 6), f);
	    putc(i1 | (i2 << 4), f);
	    break; }

	case FusionP: {
	    int nd = 0;
	    int r = xyztond(cmd->FusionP_nd.x, cmd->FusionP_nd.y, cmd->FusionP_nd.z, &nd);

	    if (r)
		return ERR_RANGE;
    
	    putc(0x07 | (nd << 3), f);
	    break; }

	case FusionS: {
	    int nd = 0;
	    int r = xyztond(cmd->FusionS_nd.x, cmd->FusionS_nd.y, cmd->FusionS_nd.z, &nd);

	    if (r)
		return ERR_RANGE;
    
	    putc(0x06 | (nd << 3), f);
	    break; }

	case Fission: {
	    int nd = 0;
	    int r = xyztond(cmd->Fission_nd.x, cmd->Fission_nd.y, cmd->Fission_nd.z, &nd);

	    if (r)
		return ERR_RANGE;
    
	    putc(0x05 | (nd << 3), f);
	    putc(cmd->m, f);
	    break; }

	case Fill: {
	    int nd = 0;
	    int r = xyztond(cmd->Fill_nd.x, cmd->Fill_nd.y, cmd->Fill_nd.z, &nd);

	    if (r)
		return ERR_RANGE;
    
	    putc(0x03 | (nd << 3), f);
	    break; }

	case Void: {
	    int nd = 0;
	    int r = xyztond(cmd->Void_nd.x, cmd->Void_nd.y, cmd->Void_nd.z, &nd);

	    if (r)
		return ERR_RANGE;
    
	    putc(0x02 | (nd << 3), f);
	    break; }

	case GFill: {
	    int nd = 0;
	    int r = xyztond(cmd->GFill_nd.x, cmd->GFill_nd.y, cmd->GFill_nd.z, &nd);

	    if (r)
		return ERR_RANGE;

	    int fd[3] = {0};
	    r = xyztofd(cmd->GFill_fd.x, cmd->GFill_fd.y, cmd->GFill_fd.z, fd);
    
	    putc(0x01 | (nd << 3), f);
	    putc(fd[0], f);
	    putc(fd[1], f);
	    putc(fd[2], f);
	    break; }

	case GVoid: {
	    int nd = 0;
	    int r = xyztond(cmd->GVoid_nd.x, cmd->GVoid_nd.y, cmd->GVoid_nd.z, &nd);

	    if (r)
		return ERR_RANGE;

	    int fd[3] = {0};
	    r = xyztofd(cmd->GVoid_fd.x, cmd->GVoid_fd.y, cmd->GVoid_fd.z, fd);
    
	    putc(0x00 | (nd << 3), f);
	    putc(fd[0], f);
	    putc(fd[1], f);
	    putc(fd[2], f);
	    break; }

	default:
	    return ERR_BAD;
	}
	return SUCCESS;
}

#define	ARRAY_SIZE	1024

int
read_trace(FILE *f, trace_t *t)
{
	GArray *a = g_array_sized_new(FALSE, FALSE, sizeof(command_t), ARRAY_SIZE);

	while (1) {
	    command_t cmd = { 0 };
	    int r = read_nbt_command(f, &cmd);   

	    if (r == ERR_EOF)
			break;
	    else if (r != SUCCESS) {
			g_array_free(a, TRUE);
			return r;
		}

	    g_array_append_val(a, cmd); 
	}

	t->n_commands = a->len;
	t->commands = (command_t*)g_array_free(a, FALSE);

	return SUCCESS;
}

int
write_trace(FILE *f, trace_t t)
{
	for (int i=0; i<t.n_commands; i++) {
	    command_t *cmd = &t.commands[i];
	    
	    int r = write_nbt_command(f, cmd);
	    if (r != SUCCESS) {
			return r;
	    }
	}
	return SUCCESS;
}
