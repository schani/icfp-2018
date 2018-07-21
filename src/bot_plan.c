#include <gmodule.h>
#include <stdlib.h>

#include "state.h"
#include "area.h"
#include "bot_plan.h"

int get_potential(GArray* areas, int i) {
    area_t a = (area_t)g_array_index(areas, area_t, i);
    return a.potential;
}

static bot_partition_t 
make_partition(int n_areas, int n_bots) {
    bot_partition_t p;
    p.n_areas = n_areas;
    p.area_potentials = calloc(n_areas, sizeof(int));
    p.area_n_bots = calloc(n_areas, sizeof(int));
    p.n_bots = n_bots;
    p.bots = calloc(n_bots, sizeof(bot_t*));
    return p;
}

static int
get_max_cost_area_index(const bot_partition_t* p) {
    int max_cost = -1;
    int max_index = -1;
    for (int i=0; i<p->n_areas; i++) {
        int cost_per_bot = p->area_potentials[i]/p->area_n_bots[i];
        if (max_cost < cost_per_bot) {
            max_cost = cost_per_bot;
            max_index = i;
        }
    }
    return max_index;
}

static int
get_bot_distance_from_area(bot_t* bot, area_t* area) { 
    region_t bb = area->bounding_box;
    coord_t mid_point = create_coord(
        (bb.c_min.x + bb.c_max.x)/2, 
        (bb.c_min.y + bb.c_max.y)/2, 
        (bb.c_min.z + bb.c_max.z)/2
    );
    return get_mdist(bot->pos, mid_point);
}

static int 
get_bot_closest_to_area(bot_partition_t *p, int offset, area_t* area) {
    int closest_index = offset;
    bot_t* closest_bot = p->bots[closest_index];
    int closest_bot_distance = get_bot_distance_from_area(closest_bot, area);
    for (offset++; offset < p->n_bots; offset++) {
        int bot_distance = get_bot_distance_from_area(closest_bot, area);
        if (bot_distance < closest_bot_distance) {
            closest_index = offset;
            closest_bot = p->bots[closest_index];
            closest_bot_distance = closest_bot_distance;
        }
    }
    return closest_index;
}

bot_partition_t
partition_bots(GArray* areas, const state_t* s) {
    // let's assume our problem is simple
    assert(areas->len <= s->n_bots);
    bot_partition_t p = make_partition(areas->len, s->n_bots);

    for (int i = 0; i < p.n_areas; i++) {
        p.area_potentials[i] = get_potential(areas, i);
        p.area_n_bots[i] = 1;
    }

    for (int i = p.n_areas; i < p.n_bots; i++) {
        int max_index = get_max_cost_area_index(&p);
        p.area_n_bots[max_index] += 1;

    }

    // tentative assignment, will swap below
    for (int i = 0; i < p.n_bots; i++) {
        p.bots[i] = &(s->bots[i]); 
    }

    // assigned count to each area; find closest bots for each area
    int bot_index = 0;
    for (int i=0; i < p.n_areas; i++) {
        area_t area = (area_t)g_array_index(areas, area_t, i);
        for (int b=0; b < p.area_n_bots[i]; b++) {
            int closest_index = get_bot_closest_to_area(&p, bot_index, &area);
            bot_t* old_bot = p.bots[bot_index];
            p.bots[bot_index] = p.bots[closest_index];
            p.bots[closest_index] = old_bot;
            assert(bot_index < p.n_bots);
            bot_index++;            
        }
    }
    assert(bot_index == p.n_bots);
    return p;
}