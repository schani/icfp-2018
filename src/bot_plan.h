#ifndef __BOT_PLAN_H
#define __BOT_PLAN_H
#include <gmodule.h>
#include "state.h"

typedef struct {
    int n_areas;
    int *area_potentials;
    int *area_n_bots;
    int n_bots;
    bot_t** bots;
} bot_partition_t;

bot_partition_t
partition_bots(GArray* areas, const state_t* s);

#endif // __BOT_PLAN_H