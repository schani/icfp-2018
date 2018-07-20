#ifndef _TRACE_H
#define _TRACE_H

#include <stdint.h>

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

typedef struct {
    coord_t c_min, c_max;
} region_t;

region_t make_region (coord_t c1, coord_t c2);

#define FOR_EACH_COORD(c, r) \
    { \
        coord_t c; \
        for (xyz_t _y = r.c_min.y; _y <= r.c_max.y; _y++) { \
            for (xyz_t _x = r.c_min.x; _x <= r.c_max.x; _x++) { \
                for (xyz_t _z = r.c_min.z; _z <= r.c_max.z; _z++) { \
                    c.x = _x; c.y = _y; c.z = _z;

#define END_FOR_EACH_COORD \
                } \
            } \
        } \
    }

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

typedef struct {
    int n_commands;
    command_t *commands;
} timestep_t;

typedef struct {
    int n_timesteps;
    timestep_t *timesteps;
} trace_t;

#endif /* _TRACE_H */
