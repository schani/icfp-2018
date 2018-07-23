#ifndef _TRACE_H
#define _TRACE_H

#include <stdint.h>
#include "coord.h"
#include "commands.h"
#include "region.h"

typedef struct {
    int n_commands;
    command_t *commands;
} trace_t;

#endif /* _TRACE_H */
