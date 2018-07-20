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

typedef struct {
    int16_t x, y, z;
} coord_t;

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
