#ifndef _COMMANDS_H
#define _COMMANDS_H


#include "coord.h"


typedef enum {
    Halt,
    Wait,
    Flip,
    SMove,
    LMove,
    Fission,
    Fill,
    FusionP,
    FusionS
} command_type_t;


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

#define FusionP_nd coord1
#define FusionS_nd coord1



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
smove_cmd(coord_t lld)
{
    assert(is_lld(lld));

	command_t cmd;
    cmd.type = SMove;
    cmd.SMove_lld = lld;
    return cmd;
}

static inline command_t
flip_cmd()
{
	command_t cmd;
    cmd.type = Flip;
    return cmd;
}

static inline void
add_cmd(GArray *cmds, command_t cmd)
{
	g_array_append_val(cmds, cmd);
}

#endif /* _COMMANDS_H */