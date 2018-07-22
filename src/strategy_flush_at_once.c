#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "default_trace.h"

#include "move_helper.h"
#include "multi_bot_helpers.h"
#include "strategy_flush_at_once.h"






void wait_n_rounds(GArray *cmds, int n){
    assert(n>=0);
    for(int i=0;i<n;++i){
        add_cmd(cmds, wait_cmd());
    }
}



void
initial_spawn(matrix_t *mdl, multi_bot_commands_t *mbc, coord_t spawn_vect){
    /* spawn the new bot */
    mbc->bot_commands[1] = fission(&mbc->bot_commands[0], spawn_vect);
}

void
move_b2_to_pos_and_duplicate(matrix_t *mdl, multi_bot_commands_t *mbc, coord_t b2_target, coord_t spawn_vect){

    /* move bot 2 to correct position and keep bot 1 waiting */
    int wait_time = goto_rel_pos(sub_coords(b2_target, get_bot_pos(&mbc->bot_commands[1])), mbc->bot_commands[1].cmds);
    set_bot_pos(&mbc->bot_commands[1], b2_target);
    wait_n_rounds(mbc->bot_commands[0].cmds, wait_time);

    /* spawn the new bots */
    mbc->bot_commands[2] = fission(&mbc->bot_commands[0], spawn_vect);
    mbc->bot_commands[3] = fission(&mbc->bot_commands[1], spawn_vect);


}

void
move_b3b4_to_pos_and_duplicate(matrix_t *mdl, multi_bot_commands_t *mbc, coord_t b3_target, coord_t b4_target, coord_t spawn_vect){

    int wait_time;
    /* move bot 3 and bot 4 to correct position and keep bot 1 and bot 2 waiting */
    int wait_time3 = goto_rel_pos(sub_coords(b3_target, get_bot_pos(&mbc->bot_commands[2])), mbc->bot_commands[2].cmds);
    set_bot_pos(&mbc->bot_commands[2], b3_target);
    int wait_time4 = goto_rel_pos(sub_coords(b4_target, get_bot_pos(&mbc->bot_commands[3])), mbc->bot_commands[3].cmds);
    set_bot_pos(&mbc->bot_commands[3], b4_target);
    
    wait_time = MAX(wait_time3, wait_time4);

    if(wait_time3 < wait_time){
        wait_n_rounds(mbc->bot_commands[2].cmds, wait_time-wait_time3);
    }
    if(wait_time4 < wait_time){
        wait_n_rounds(mbc->bot_commands[3].cmds, wait_time-wait_time4);
    }

    wait_n_rounds(mbc->bot_commands[0].cmds, wait_time);
    wait_n_rounds(mbc->bot_commands[1].cmds, wait_time);

    /* spawn the new bots */
    mbc->bot_commands[4] = fission(&mbc->bot_commands[0], spawn_vect);
    mbc->bot_commands[5] = fission(&mbc->bot_commands[1], spawn_vect);
    mbc->bot_commands[6] = fission(&mbc->bot_commands[2], spawn_vect);
    mbc->bot_commands[7] = fission(&mbc->bot_commands[3], spawn_vect);

}

void debugBotPos(bot_t * bot){
    printf("BotId: %d\n", bot->bid);
    printf("BotPos: %d %d %d\n", bot->pos.x, bot->pos.y, bot->pos.z);
}


void
move_b5b6b7b8_to_pos_and_void(matrix_t *mdl, multi_bot_commands_t *mbc, coord_t b5_target, coord_t b6_target, coord_t b7_target, coord_t b8_target){
    int wait_time;
    int wait_time_a;
    int wait_time_b;

    /* move bot 5, bot 6, bot 7, and bot 8 to correct position and keep others waiting */
    int wait_time5 = goto_rel_pos(sub_coords(b5_target, get_bot_pos(&mbc->bot_commands[4])), mbc->bot_commands[4].cmds);
    set_bot_pos(&mbc->bot_commands[4], b5_target);
    int wait_time6 = goto_rel_pos(sub_coords(b6_target, get_bot_pos(&mbc->bot_commands[5])), mbc->bot_commands[5].cmds);
    set_bot_pos(&mbc->bot_commands[5], b6_target);
    int wait_time7 = goto_rel_pos(sub_coords(b7_target, get_bot_pos(&mbc->bot_commands[6])), mbc->bot_commands[6].cmds);
    set_bot_pos(&mbc->bot_commands[6], b7_target);
    int wait_time8 = goto_rel_pos(sub_coords(b8_target, get_bot_pos(&mbc->bot_commands[7])), mbc->bot_commands[7].cmds);
    set_bot_pos(&mbc->bot_commands[7], b8_target);

    wait_time_a = MAX(wait_time5, wait_time6);
    
    wait_time_b = MAX(wait_time7, wait_time8);
    
    wait_time = MAX(wait_time_a, wait_time_b);
    
    
    if(wait_time5 < wait_time){
        wait_n_rounds(mbc->bot_commands[4].cmds, wait_time-wait_time5);
    }
    if(wait_time6 < wait_time){
        wait_n_rounds(mbc->bot_commands[5].cmds, wait_time-wait_time6);
    }
    if(wait_time7 < wait_time){
        wait_n_rounds(mbc->bot_commands[6].cmds, wait_time-wait_time7);
    }
    if(wait_time8 < wait_time){
        wait_n_rounds(mbc->bot_commands[7].cmds, wait_time-wait_time8);
    }

    wait_n_rounds(mbc->bot_commands[0].cmds, wait_time);
    wait_n_rounds(mbc->bot_commands[1].cmds, wait_time);
    wait_n_rounds(mbc->bot_commands[2].cmds, wait_time);
    wait_n_rounds(mbc->bot_commands[3].cmds, wait_time);

    /* void the region surrounded */
    add_cmd(mbc->bot_commands[0].cmds, gvoid_cmd(create_coord( 1, 0,  1), create_coord(  mdl->resolution-3,   mdl->resolution-1,   mdl->resolution-3)));
    add_cmd(mbc->bot_commands[7].cmds, gvoid_cmd(create_coord(-1, 0, -1), create_coord(-(mdl->resolution-3), -mdl->resolution+1, -(mdl->resolution-3))));

    add_cmd(mbc->bot_commands[1].cmds, gvoid_cmd(create_coord(-1, 0,  1), create_coord(-(mdl->resolution-3),  mdl->resolution-1,   mdl->resolution-3)));
    add_cmd(mbc->bot_commands[6].cmds, gvoid_cmd(create_coord( 1, 0, -1), create_coord(  mdl->resolution-3,  -mdl->resolution+1, -(mdl->resolution-3))));

    add_cmd(mbc->bot_commands[2].cmds, gvoid_cmd(create_coord( 1, 0, -1), create_coord(  mdl->resolution-3,   mdl->resolution-1, -(mdl->resolution-3))));
    add_cmd(mbc->bot_commands[5].cmds, gvoid_cmd(create_coord(-1, 0,  1), create_coord(-(mdl->resolution-3), -mdl->resolution+1,   mdl->resolution-3)));

    add_cmd(mbc->bot_commands[3].cmds, gvoid_cmd(create_coord(-1, 0, -1), create_coord(-(mdl->resolution-3),  mdl->resolution-1, -(mdl->resolution-3))));
    add_cmd(mbc->bot_commands[4].cmds, gvoid_cmd(create_coord( 1, 0,  1), create_coord(  mdl->resolution-3,  -mdl->resolution+1,   mdl->resolution-3)));

/*
    for(int i=0; i<mbc->n_bots; ++i){
        debugBotPos(&mbc->bot_commands[i].bot);
    }
*/


}


void
move_b5b6b7b8_to_pos_and_fuse(matrix_t *mdl, multi_bot_commands_t *mbc, coord_t b5_target, coord_t b6_target, coord_t b7_target, coord_t b8_target){
    int wait_time;
    int wait_time_a;
    int wait_time_b;

    /* move bot 5, bot 6, bot 7, and bot 8 to correct position and keep others waiting */
    int wait_time5 = goto_rel_pos(sub_coords(b5_target, get_bot_pos(&mbc->bot_commands[4])), mbc->bot_commands[4].cmds);
    set_bot_pos(&mbc->bot_commands[4], b5_target);
    int wait_time6 = goto_rel_pos(sub_coords(b6_target, get_bot_pos(&mbc->bot_commands[5])), mbc->bot_commands[5].cmds);
    set_bot_pos(&mbc->bot_commands[5], b6_target);
    int wait_time7 = goto_rel_pos(sub_coords(b7_target, get_bot_pos(&mbc->bot_commands[6])), mbc->bot_commands[6].cmds);
    set_bot_pos(&mbc->bot_commands[6], b7_target);
    int wait_time8 = goto_rel_pos(sub_coords(b8_target, get_bot_pos(&mbc->bot_commands[7])), mbc->bot_commands[7].cmds);
    set_bot_pos(&mbc->bot_commands[7], b8_target);

    wait_time_a = MAX(wait_time5, wait_time6);
    wait_time_b = MAX(wait_time7, wait_time8);
    wait_time = MAX(wait_time_a, wait_time_b);
    
    if(wait_time5 < wait_time){
        wait_n_rounds(mbc->bot_commands[4].cmds, wait_time-wait_time5);
    }
    if(wait_time6 < wait_time){
        wait_n_rounds(mbc->bot_commands[5].cmds, wait_time-wait_time6);
    }
    if(wait_time7 < wait_time){
        wait_n_rounds(mbc->bot_commands[6].cmds, wait_time-wait_time7);
    }
    if(wait_time8 < wait_time){
        wait_n_rounds(mbc->bot_commands[7].cmds, wait_time-wait_time8);
    }

    wait_n_rounds(mbc->bot_commands[0].cmds, wait_time);
    wait_n_rounds(mbc->bot_commands[1].cmds, wait_time);
    wait_n_rounds(mbc->bot_commands[2].cmds, wait_time);
    wait_n_rounds(mbc->bot_commands[3].cmds, wait_time);

    /* fuse the bots */
    fusion(&mbc->bot_commands[0], &mbc->bot_commands[4]);
    fusion(&mbc->bot_commands[1], &mbc->bot_commands[5]);
    fusion(&mbc->bot_commands[2], &mbc->bot_commands[6]);
    fusion(&mbc->bot_commands[3], &mbc->bot_commands[7]);

}


void
move_b3b4_to_pos_and_fuse(matrix_t *mdl, multi_bot_commands_t *mbc, coord_t b3_target, coord_t b4_target){

    int wait_time;
    /* move bot 3 and bot 4 to correct position and keep bot 1 and bot 2 waiting */
    int wait_time3 = goto_rel_pos(sub_coords(b3_target, get_bot_pos(&mbc->bot_commands[2])), mbc->bot_commands[2].cmds);
    set_bot_pos(&mbc->bot_commands[2], b3_target);
    int wait_time4 = goto_rel_pos(sub_coords(b4_target, get_bot_pos(&mbc->bot_commands[3])), mbc->bot_commands[3].cmds);
    set_bot_pos(&mbc->bot_commands[3], b4_target);
    
    wait_time = MAX(wait_time3, wait_time4);

    if(wait_time3 < wait_time){
        wait_n_rounds(mbc->bot_commands[2].cmds, wait_time-wait_time3);
    }
    if(wait_time4 < wait_time){
        wait_n_rounds(mbc->bot_commands[3].cmds, wait_time-wait_time4);
    }

    wait_n_rounds(mbc->bot_commands[0].cmds, wait_time);
    wait_n_rounds(mbc->bot_commands[1].cmds, wait_time);

    /* fuse the bots */
    fusion(&mbc->bot_commands[0], &mbc->bot_commands[2]);
    fusion(&mbc->bot_commands[1], &mbc->bot_commands[3]);

}


void
move_b2_to_pos_and_fuse(matrix_t *mdl, multi_bot_commands_t *mbc, coord_t b2_target){
    

    /* move bot 2 to correct position and keep bot 1 waiting */
    int wait_time = goto_rel_pos(sub_coords(b2_target, get_bot_pos(&mbc->bot_commands[1])), mbc->bot_commands[1].cmds);
    set_bot_pos(&mbc->bot_commands[1], b2_target);
    wait_n_rounds(mbc->bot_commands[0].cmds, wait_time);

    /* fuse the bots */
    fusion(&mbc->bot_commands[0], &mbc->bot_commands[1]);

}

GArray* 
exec_flush_at_once(matrix_t *mdl, bot_t *bot1){

    /* only applicabl for small resolutions */
    assert(mdl->resolution<=30);


    multi_bot_commands_t mbc = make_multi_bot_commands(8);

    mbc.bot_commands[0] = make_bot_commands(*bot1);

    initial_spawn(mdl, &mbc, create_coord(1,0,0));
    move_b2_to_pos_and_duplicate(mdl, &mbc, create_coord(mdl->resolution-1,0,0), create_coord(0,0,1));
    move_b3b4_to_pos_and_duplicate(mdl, &mbc, 
    create_coord(0,0,mdl->resolution-1), create_coord(mdl->resolution-1,0,mdl->resolution-1), create_coord(0,1,0));

    move_b5b6b7b8_to_pos_and_void(mdl, &mbc, 
        create_coord(0,mdl->resolution-1,0),    
        create_coord(mdl->resolution-1,mdl->resolution-1,0),
        create_coord(0,mdl->resolution-1,mdl->resolution-1),
        create_coord(mdl->resolution-1,mdl->resolution-1,mdl->resolution-1));

    move_b5b6b7b8_to_pos_and_fuse(mdl, &mbc,
            create_coord(0,1,0),    
            create_coord(mdl->resolution-1,1,0),
            create_coord(0,1,mdl->resolution-1),
            create_coord(mdl->resolution-1,1,mdl->resolution-1));

    move_b3b4_to_pos_and_fuse(mdl, &mbc,
            create_coord(0,0,1), 
            create_coord(mdl->resolution-1,0,1));

    move_b2_to_pos_and_fuse(mdl, &mbc, 
            create_coord(1,0,0));

	add_cmd(mbc.bot_commands[0].cmds, halt_cmd());

    return merge_bot_commands(mbc);

}


