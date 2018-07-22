#ifndef _COMMANDS_H
#define _COMMANDS_H

#include <stdint.h>

#include "coord.h"


typedef enum {
    Halt,
    Wait,
    Flip,
    SMove,
    LMove,
    Fission,
    Fill,
    Void,
    FusionP,
    FusionS,
    GFill,
    GVoid
} command_type_t;
#define COMMAND_MAX GVoid


typedef struct {
    command_type_t type;
    coord_t coord1;
    coord_t coord2;
    uint8_t m;
} command_t;

#define SMove_lld coord1

#define LMove_sld1 coord1
#define LMove_sld2 coord2

#define Fission_nd coord1

#define Fill_nd coord1
#define Void_nd coord1

#define FusionP_nd coord1
#define FusionS_nd coord1

#define GFill_nd coord1
#define GFill_fd coord2

#define GVoid_nd coord1
#define GVoid_fd coord2


void print_cmd (command_t cmd);


static inline command_t 
halt_cmd(void)
{
	command_t cmd;
    cmd.type = Halt;
    return cmd;
}

static inline command_t 
fill_cmd(coord_t nd)
{
    assert(is_nd(nd));

	command_t cmd;
	cmd.type = Fill;
	cmd.Fill_nd = nd;
	return cmd;
}

static inline command_t 
void_cmd(coord_t nd)
{
    assert(is_nd(nd));

	command_t cmd;
	cmd.type = Void;
	cmd.Void_nd = nd;
	return cmd;
}

static inline command_t
smove_cmd(coord_t lld)
{
    assert(is_lld(lld));

	command_t cmd;
    cmd.type = SMove;
    cmd.SMove_lld = lld;
    return cmd;
}

static inline command_t
lmove_cmd(coord_t sld1, coord_t sld2)
{
    assert(is_sld(sld1));
    assert(is_sld(sld2));

	command_t cmd;
    cmd.type = LMove;
    cmd.LMove_sld1 = sld1;
    cmd.LMove_sld2 = sld2;
    return cmd;
}

static inline command_t
fission_cmd(coord_t nd, int m)
{
    assert(is_nd(nd));
    assert(m>=0);

	command_t cmd;
    cmd.type = Fission;
    cmd.Fission_nd = nd;
    cmd.m = m;
    return cmd;
}

static inline command_t
gfill_cmd(coord_t nd, coord_t fd)
{
    assert(is_nd(nd));
    assert(is_fd(fd));

	command_t cmd;
    cmd.type = GFill;
    cmd.GFill_nd = nd;
    cmd.GFill_fd = fd;
    return cmd;
}

static inline command_t
gvoid_cmd(coord_t nd, coord_t fd)
{
    assert(is_nd(nd));
    assert(is_fd(fd));

	command_t cmd;
    cmd.type = GVoid;
    cmd.GVoid_nd = nd;
    cmd.GVoid_fd = fd;
    return cmd;
}

static inline command_t
flip_cmd()
{
	command_t cmd;
    cmd.type = Flip;
    return cmd;
}

static inline command_t
wait_cmd()
{
	command_t cmd;
    cmd.type = Wait;
    return cmd;
}

static inline command_t
fusionp_cmd(coord_t nd)
{
    command_t cmd;
    cmd.type = FusionP;
    cmd.FusionP_nd = nd;
    return cmd;
}

static inline command_t
fusions_cmd(coord_t nd) 
{
    command_t cmd;
    cmd.type = FusionS;
    cmd.FusionS_nd = nd;  
    return cmd;  
}

static inline void
add_cmd(GArray *cmds, command_t cmd)
{
	assert(cmd.type >= 0);
    assert(cmd.type <= COMMAND_MAX);
	g_array_append_val(cmds, cmd);
}

#endif /* _COMMANDS_H */
