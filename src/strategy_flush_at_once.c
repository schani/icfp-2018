#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmodule.h>
#include "default_trace.h"

#include "move_helper.h"
#include "multi_bot_helpers.h"


typedef struct{
    bot_commands_t *b1;
    bot_commands_t *b2;
} bc2_t;


typedef struct{
    bot_commands_t *b1;
    bot_commands_t *b2;
    bot_commands_t *b3;
    bot_commands_t *b4;
} bc4_t;

typedef struct{
    bot_commands_t *b1;
    bot_commands_t *b2;
    bot_commands_t *b3;
    bot_commands_t *b4;
    bot_commands_t *b5;
    bot_commands_t *b6;
    bot_commands_t *b7;
    bot_commands_t *b8;
} bc8_t;





void wait_n_rounds(GArray *cmds, int n){
    assert(n>=0);
    for(int i=0;i<n;++i){
        add_cmd(cmds, wait_cmd());
    }
}



bc2_t
initial_spawn(matrix_t *mdl, bot_commands_t *b1, coord_t spawn_vect){
    bc2_t ret_val;
    ret_val.b1 = b1;

    /* spawn the new bot */
    *ret_val.b2 = fission(b1, spawn_vect);

    return ret_val;
}

bc4_t
move_b2_to_pos_and_duplicate(matrix_t *mdl, bc2_t bot_commands, coord_t b2_target, coord_t spawn_vect){
    bc4_t ret_val;

    ret_val.b1 = bot_commands.b1;
    ret_val.b2 = bot_commands.b2;


    /* move bot 2 to correct position and keep bot 1 waiting */
    int wait_time = goto_rel_pos(sub_coords(b2_target, get_bot_pos(bot_commands.b2)), bot_commands.b2->cmds);
    set_bot_pos(bot_commands.b2, b2_target);
    wait_n_rounds(bot_commands.b1->cmds, wait_time);

    /* spawn the new bots */
    *ret_val.b3 = fission(bot_commands.b1, spawn_vect);
    *ret_val.b4 = fission(bot_commands.b2, spawn_vect);

    return ret_val;
}

bc8_t
move_b3b4_to_pos_and_duplicate(matrix_t *mdl, bc4_t bot_commands, coord_t b3_target, coord_t b4_target, coord_t spawn_vect){
    bc8_t ret_val;

    ret_val.b1 = bot_commands.b1;
    ret_val.b2 = bot_commands.b2;
    ret_val.b3 = bot_commands.b3;
    ret_val.b4 = bot_commands.b4;

    int wait_time;
    /* move bot 3 and bot 4 to correct position and keep bot 1 and bot 2 waiting */
    int wait_time3 = goto_rel_pos(sub_coords(b3_target, get_bot_pos(bot_commands.b3)), bot_commands.b3->cmds);
    set_bot_pos(bot_commands.b3, b3_target);
    int wait_time4 = goto_rel_pos(sub_coords(b4_target, get_bot_pos(bot_commands.b4)), bot_commands.b4->cmds);
    set_bot_pos(bot_commands.b4, b4_target);
    
    wait_time = MAX(wait_time3, wait_time4);

    if(wait_time3 < wait_time){
        wait_n_rounds(ret_val.b3->cmds, wait_time-wait_time3);
    }
    if(wait_time4 < wait_time){
        wait_n_rounds(ret_val.b4->cmds, wait_time-wait_time4);
    }

    wait_n_rounds(bot_commands.b1->cmds, wait_time);
    wait_n_rounds(bot_commands.b2->cmds, wait_time);

    /* spawn the new bots */
    *ret_val.b5 = fission(bot_commands.b1, spawn_vect);
    *ret_val.b6 = fission(bot_commands.b2, spawn_vect);
    *ret_val.b7 = fission(bot_commands.b3, spawn_vect);
    *ret_val.b8 = fission(bot_commands.b4, spawn_vect);

    return ret_val;

}

void
move_b5b6b7b8_to_pos_and_void(matrix_t *mdl, bc8_t bot_commands, coord_t b5_target, coord_t b6_target, coord_t b7_target, coord_t b8_target){
    int wait_time;
    int wait_time_a;
    int wait_time_b;

    /* move bot 5, bot 6, bot 7, and bot 8 to correct position and keep others waiting */
    int wait_time5 = goto_rel_pos(sub_coords(b5_target, get_bot_pos(bot_commands.b5)), bot_commands.b5->cmds);
    set_bot_pos(bot_commands.b5, b5_target);
    int wait_time6 = goto_rel_pos(sub_coords(b6_target, get_bot_pos(bot_commands.b6)), bot_commands.b6->cmds);
    set_bot_pos(bot_commands.b6, b6_target);
    int wait_time7 = goto_rel_pos(sub_coords(b7_target, get_bot_pos(bot_commands.b7)), bot_commands.b7->cmds);
    set_bot_pos(bot_commands.b7, b7_target);
    int wait_time8 = goto_rel_pos(sub_coords(b8_target, get_bot_pos(bot_commands.b8)), bot_commands.b8->cmds);
    set_bot_pos(bot_commands.b8, b8_target);

    wait_time_a = MAX(wait_time5, wait_time6);
    wait_time_b = MAX(wait_time7, wait_time8);
    wait_time = MAX(wait_time_a, wait_time_b);
    
    if(wait_time5 < wait_time){
        wait_n_rounds(bot_commands.b5->cmds, wait_time-wait_time5);
    }
    if(wait_time6 < wait_time){
        wait_n_rounds(bot_commands.b6->cmds, wait_time-wait_time6);
    }
    if(wait_time7 < wait_time){
        wait_n_rounds(bot_commands.b7->cmds, wait_time-wait_time7);
    }
    if(wait_time8 < wait_time){
        wait_n_rounds(bot_commands.b8->cmds, wait_time-wait_time8);
    }

    wait_n_rounds(bot_commands.b1->cmds, wait_time);
    wait_n_rounds(bot_commands.b2->cmds, wait_time);
    wait_n_rounds(bot_commands.b3->cmds, wait_time);
    wait_n_rounds(bot_commands.b4->cmds, wait_time);

    /* void the region surrounded */
    add_cmd(bot_commands.b1->cmds, gvoid_cmd(create_coord( 1, 0,  1), create_coord( mdl->resolution-3,  mdl->resolution-1,  mdl->resolution-3)));
    add_cmd(bot_commands.b8->cmds, gvoid_cmd(create_coord(-1, 0, -1), create_coord(-mdl->resolution-3, -mdl->resolution-1, -mdl->resolution-3)));

    add_cmd(bot_commands.b2->cmds, gvoid_cmd(create_coord(-1, 0,  1), create_coord(-mdl->resolution-3,  mdl->resolution-1,  mdl->resolution-3)));
    add_cmd(bot_commands.b7->cmds, gvoid_cmd(create_coord( 1, 0, -1), create_coord( mdl->resolution-3, -mdl->resolution-1, -mdl->resolution-3)));

    add_cmd(bot_commands.b3->cmds, gvoid_cmd(create_coord( 1, 0, -1), create_coord( mdl->resolution-3,  mdl->resolution-1, -mdl->resolution-3)));
    add_cmd(bot_commands.b6->cmds, gvoid_cmd(create_coord(-1, 0,  1), create_coord(-mdl->resolution-3, -mdl->resolution-1,  mdl->resolution-3)));

    add_cmd(bot_commands.b4->cmds, gvoid_cmd(create_coord(-1, 0, -1), create_coord(-mdl->resolution-3,  mdl->resolution-1, -mdl->resolution-3)));
    add_cmd(bot_commands.b5->cmds, gvoid_cmd(create_coord( 1, 0,  1), create_coord( mdl->resolution-3, -mdl->resolution-1,  mdl->resolution-3)));

}


void
move_b5b6b7b8_to_pos_and_fuse(matrix_t *mdl, bc8_t bot_commands, coord_t b5_target, coord_t b6_target, coord_t b7_target, coord_t b8_target){
    int wait_time;
    int wait_time_a;
    int wait_time_b;

    /* move bot 5, bot 6, bot 7, and bot 8 to correct position and keep others waiting */
    int wait_time5 = goto_rel_pos(sub_coords(b5_target, get_bot_pos(bot_commands.b5)), bot_commands.b5->cmds);
    set_bot_pos(bot_commands.b5, b5_target);
    int wait_time6 = goto_rel_pos(sub_coords(b6_target, get_bot_pos(bot_commands.b6)), bot_commands.b6->cmds);
    set_bot_pos(bot_commands.b6, b6_target);
    int wait_time7 = goto_rel_pos(sub_coords(b7_target, get_bot_pos(bot_commands.b7)), bot_commands.b7->cmds);
    set_bot_pos(bot_commands.b7, b7_target);
    int wait_time8 = goto_rel_pos(sub_coords(b8_target, get_bot_pos(bot_commands.b8)), bot_commands.b8->cmds);
    set_bot_pos(bot_commands.b8, b8_target);

    wait_time_a = MAX(wait_time5, wait_time6);
    wait_time_b = MAX(wait_time7, wait_time8);
    wait_time = MAX(wait_time_a, wait_time_b);
    
    if(wait_time5 < wait_time){
        wait_n_rounds(bot_commands.b5->cmds, wait_time-wait_time5);
    }
    if(wait_time6 < wait_time){
        wait_n_rounds(bot_commands.b6->cmds, wait_time-wait_time6);
    }
    if(wait_time7 < wait_time){
        wait_n_rounds(bot_commands.b7->cmds, wait_time-wait_time7);
    }
    if(wait_time8 < wait_time){
        wait_n_rounds(bot_commands.b8->cmds, wait_time-wait_time8);
    }

    wait_n_rounds(bot_commands.b1->cmds, wait_time);
    wait_n_rounds(bot_commands.b2->cmds, wait_time);
    wait_n_rounds(bot_commands.b3->cmds, wait_time);
    wait_n_rounds(bot_commands.b4->cmds, wait_time);

    /* fuse the bots */
    fusion(bot_commands.b1, bot_commands.b5);
    fusion(bot_commands.b2, bot_commands.b6);
    fusion(bot_commands.b3, bot_commands.b7);
    fusion(bot_commands.b4, bot_commands.b8);

}


void
move_b3b4_to_pos_and_fuse(matrix_t *mdl, bc8_t bot_commands, coord_t b3_target, coord_t b4_target){

    int wait_time;
    /* move bot 3 and bot 4 to correct position and keep bot 1 and bot 2 waiting */
    int wait_time3 = goto_rel_pos(sub_coords(b3_target, get_bot_pos(bot_commands.b3)), bot_commands.b3->cmds);
    set_bot_pos(bot_commands.b3, b3_target);
    int wait_time4 = goto_rel_pos(sub_coords(b4_target, get_bot_pos(bot_commands.b4)), bot_commands.b4->cmds);
    set_bot_pos(bot_commands.b4, b4_target);
    
    wait_time = MAX(wait_time3, wait_time4);

    if(wait_time3 < wait_time){
        wait_n_rounds(bot_commands.b3->cmds, wait_time-wait_time3);
    }
    if(wait_time4 < wait_time){
        wait_n_rounds(bot_commands.b4->cmds, wait_time-wait_time4);
    }

    wait_n_rounds(bot_commands.b1->cmds, wait_time);
    wait_n_rounds(bot_commands.b2->cmds, wait_time);

    /* fuse the bots */
    fusion(bot_commands.b1, bot_commands.b3);
    fusion(bot_commands.b2, bot_commands.b4);

}


void
move_b2_to_pos_and_fuse(matrix_t *mdl, bc8_t bot_commands, coord_t b2_target){
    

    /* move bot 2 to correct position and keep bot 1 waiting */
    int wait_time = goto_rel_pos(sub_coords(b2_target, get_bot_pos(bot_commands.b2)), bot_commands.b2->cmds);
    set_bot_pos(bot_commands.b2, b2_target);
    wait_n_rounds(bot_commands.b1->cmds, wait_time);

    /* fuse the bots */
    fusion(bot_commands.b1, bot_commands.b2);

}

GArray* 
exec_flush_at_once(matrix_t *mdl, bot_t bot1){

    /* only applicabl for small resolutions */
    assert(mdl->resolution<=30);


    bot_commands_t bot1_commands = make_bot_commands(bot1);

    bc2_t exec_2bots = initial_spawn(mdl, &bot1_commands, create_coord(1,0,0));
    bc4_t exec_4bots = move_b2_to_pos_and_duplicate(mdl, exec_2bots, create_coord(mdl->resolution-1,0,0), create_coord(0,0,1));
    bc8_t exec_8bots = move_b3b4_to_pos_and_duplicate(mdl, exec_4bots, 
    create_coord(0,0,mdl->resolution-1), create_coord(mdl->resolution-1,0,mdl->resolution-1), create_coord(0,1,0));

    move_b5b6b7b8_to_pos_and_void(mdl, exec_8bots, 
        create_coord(0,mdl->resolution-1,0),    
        create_coord(mdl->resolution-1,mdl->resolution-1,0),
        create_coord(0,mdl->resolution-1,mdl->resolution-1),
        create_coord(mdl->resolution-1,mdl->resolution-1,mdl->resolution-1));

    move_b5b6b7b8_to_pos_and_fuse(mdl, exec_8bots,
            create_coord(0,1,0),    
            create_coord(mdl->resolution-1,1,0),
            create_coord(0,1,mdl->resolution-1),
            create_coord(mdl->resolution-1,1,mdl->resolution-1));

    move_b3b4_to_pos_and_fuse(mdl, exec_8bots,
            create_coord(0,0,1), 
            create_coord(mdl->resolution-1,0,1));

    move_b2_to_pos_and_fuse(mdl, exec_8bots, 
            create_coord(1,0,0));

	add_cmd(bot1_commands.cmds, halt_cmd());


    /* FIXME : call the exec merger  */



}


