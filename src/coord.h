#ifndef _COORD_H
#define _COORD_H


#include <glib.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef int16_t xyz_t;

typedef struct {
    xyz_t x, y, z;
} coord_t;

static inline coord_t
add_coords (coord_t c1, coord_t c2) {
    coord_t c;
    c.x = c1.x + c2.x;
    c.y = c1.y + c2.y;
    c.z = c1.z + c2.z;
    return c;
}

// FIXME: rename to make_coord and make public
static inline coord_t 
create_coord(int16_t x, int16_t y, int16_t z)
{
	coord_t c;
	c.x = x;
	c.y = y;
	c.z = z;
	return c;
}

/* is linear coordinate difference */
static inline bool
is_lcd(coord_t c){
    return ((c.x!=0 && c.y==0 && c.z == 0) || 
            (c.x==0 && c.y!=0 && c.z == 0) || 
            (c.x==0 && c.y==0 && c.z != 0));
}


/* returns Manhattan length of a difference */
static inline int 
get_mlen(coord_t c){
    return (abs(c.x) + abs(c.y) + abs(c.z));
}

/* returns Chessboard length of a difference */
static inline int 
get_clen(coord_t c){
    return ( MAX ( MAX( abs(c.x),  abs(c.y)),  abs(c.z)));
}

/* is long linear coordinate difference*/
static inline bool
is_lld(coord_t c){
    return (is_lcd(c) && get_mlen(c) <= 15);
}

/* is short linear coordinate difference*/
static inline bool
is_sld(coord_t c){
    return (is_lcd(c) && get_mlen(c) <= 5);
}

/* is near linear coordinate difference*/
static inline bool
is_nd(coord_t c){
    return (get_mlen(c) > 0 && get_mlen(c) <=2 && get_clen(c) == 1);
}



#endif /* _COORD_H */
