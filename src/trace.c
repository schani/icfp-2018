#include <glib.h>

#include "trace.h"

region_t
make_region (coord_t c1, coord_t c2) {
    region_t r;
    r.c_min.x = MIN(c1.x, c2.x);
    r.c_max.x = MAX(c1.x, c2.x);
    r.c_min.y = MIN(c1.y, c2.y);
    r.c_max.y = MAX(c1.y, c2.y);
    r.c_min.z = MIN(c1.z, c2.z);
    r.c_max.z = MAX(c1.z, c2.z);
    return r;
}
