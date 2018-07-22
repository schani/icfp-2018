#include <stdio.h>

#include "commands.h"

void print_cmd(command_t cmd){
    switch (cmd.type) {
    case Halt:
        printf("Halt\n");
        break;

    case Wait:
        printf("Wait\n");
        break;

    case Flip:
        printf("Flip\n");
        break;

    case SMove: {
        printf("SMove   %d %d %d\n", cmd.SMove_lld.x, cmd.SMove_lld.y, cmd.SMove_lld.z);
        break;
    }

    case LMove: {
        printf("LMove   %d %d %d   %d %d %d\n", cmd.LMove_sld1.x, cmd.LMove_sld1.y, cmd.LMove_sld1.z, cmd.LMove_sld2.x, cmd.LMove_sld2.y, cmd.LMove_sld2.z);
        break;
    }

    case Fission: {
        printf("Fission %d %d %d  %d\n", cmd.Fission_nd.x, cmd.Fission_nd.y, cmd.Fission_nd.z, cmd.m);
        break;
    }

    case Fill: {
        printf("Fill    %d %d %d\n", cmd.Fill_nd.x, cmd.Fill_nd.y, cmd.Fill_nd.z);
        break;
    }

    case Void: {
        printf("Void    %d %d %d\n", cmd.Void_nd.x, cmd.Void_nd.y, cmd.Void_nd.z);
        break;
    }

    case FusionP: {
        printf("FusionP %d %d %d\n", cmd.FusionP_nd.x, cmd.FusionP_nd.y, cmd.FusionP_nd.z);
        break;
    }

    case FusionS: {
        printf("FusionS %d %d %d\n", cmd.FusionS_nd.x, cmd.FusionS_nd.y, cmd.FusionS_nd.z);
        break;
    }

    case GFill: {
        printf("GFill %d %d %d   %d %d %d\n", cmd.GFill_nd.x, cmd.GFill_nd.y, cmd.GFill_nd.z, cmd.GFill_fd.x, cmd.GFill_fd.y, cmd.GFill_fd.z);
        break;
    }

    case GVoid: {
        printf("GVoid %d %d %d   %d %d %d\n", cmd.GVoid_nd.x, cmd.GVoid_nd.y, cmd.GVoid_nd.z, cmd.GVoid_fd.x, cmd.GVoid_fd.y, cmd.GVoid_fd.z);
        break;
    }

    default:
        assert(false);
    }
}
