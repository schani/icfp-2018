#include "move_helper.h"

#include "commands.h"

static inline xyz_t
xyz_sign(xyz_t x) {
    if(x > 0) return 1;
    if(x < 0) return -1; 
    return 0;
}

static inline xyz_t
smove_amount(xyz_t x) {
    return abs(x) > 15 ? xyz_sign(x) * 15 : x;
}

int goto_rel_pos(coord_t rel_pos, GArray *cmds) {

    int cnt_cmds = 0;
    // FIXME use LMOVE as well.

    // move in y direction first
    while(rel_pos.y != 0) { 
        coord_t lld = create_coord(0, smove_amount(rel_pos.y),0);
        add_cmd(cmds, smove_cmd(lld));
        rel_pos = sub_coords(rel_pos, lld);
        cnt_cmds += 1;
    }
    while(rel_pos.x != 0) {
        coord_t lld = create_coord(smove_amount(rel_pos.x), 0,0);
        add_cmd(cmds, smove_cmd(lld));
        rel_pos = sub_coords(rel_pos, lld);
        cnt_cmds += 1;
    }
    while(rel_pos.z != 0) {
        coord_t lld = create_coord(0, 0, smove_amount(rel_pos.z));
        add_cmd(cmds, smove_cmd(lld));
        rel_pos = sub_coords(rel_pos, lld);
        cnt_cmds += 1;
    }
    return cnt_cmds;
}

int goto_next_pos(coord_t *curPos, coord_t nextPos, GArray *cmds) {
    int cnt_cmds = 0;
    printf("goto next pos %d %d %d -> %d %d %d\n", curPos->x, curPos->y, curPos->z, nextPos.x, nextPos.y, nextPos.z);
    cnt_cmds = goto_rel_pos(sub_coords(nextPos, *curPos), cmds);
    *curPos = nextPos;
    return cnt_cmds;
}
