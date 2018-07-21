#ifndef _COORD_H
#define _COORD_H

#include <stdio.h>
#include <glib.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef int16_t xyz_t;

typedef struct {
    xyz_t x, y, z;
} coord_t;

// FIXME: rename to make_coord
static inline coord_t 
create_coord(xyz_t x, xyz_t y, xyz_t z)
{
	coord_t c;
	c.x = x;
	c.y = y;
	c.z = z;
	return c;
}

static inline coord_t
add_coords (coord_t c1, coord_t c2) {
    return create_coord(c1.x + c2.x, c1.y + c2.y, c1.z + c2.z);
}

static inline coord_t
sub_coords (coord_t c1, coord_t c2) {
    return create_coord(c1.x - c2.x, c1.y - c2.y, c1.z - c2.z);
}

static inline coord_t
add_x (coord_t c, xyz_t x) {
    return add_coords(c, create_coord(x, 0, 0));
}

static inline coord_t
add_y (coord_t c, xyz_t y) {
    return add_coords(c, create_coord(0, y, 0));
}

static inline coord_t
add_z (coord_t c, xyz_t z) {
    return add_coords(c, create_coord(0, 0, z));
}

static inline void
coord_tostring (coord_t c, char* s) {
    printf("Coordinates: %s (%i,%i,%i)\n", s, c.x, c.y, c.z);
}

static inline bool
is_coords_equal(coord_t c1, coord_t c2){
    return ((c1.x == c2.x) && (c1.y = c2.y) && (c1.z == c2.z));
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

static inline int
get_mdist(coord_t c1, coord_t c2) {
    return get_mlen(sub_coords(c1, c2));
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
