#include <gmodule.h>
#include "trace.h"
#include "model.h"
#include "coord.h"
#include "commands.h"

void calc_boundary_box(matrix_t *mdl, coord_t *minFull, coord_t *maxFull);


GArray* exec_default_trace(matrix_t *mdl);