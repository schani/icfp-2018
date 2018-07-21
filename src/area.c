#include "area.h"
#include "model.h"
#include "trace.h"

// FIXME make public
static region_t
extend_region(region_t r_in, coord_t c) {
    region_t r;
    r.c_min.x = MIN(r_in.c_min.x, c.x);
    r.c_max.x = MAX(r_in.c_max.x, c.x);
    r.c_min.y = MIN(r_in.c_min.y, c.y);
    r.c_max.y = MAX(r_in.c_max.y, c.y);
    r.c_min.z = MIN(r_in.c_min.z, c.z);
    r.c_max.z = MAX(r_in.c_max.z, c.z);
    return r;
}

static area_t
make_area(int blob_id, int phase, coord_t coord) {
    area_t a;
    a.blob_id = blob_id;
    a.phase = phase;
    a.bounding_box = make_region(coord, coord);
    a.potential = 0; // no potential here
    return a;
} 

// pointer is only valid until next insert
static area_t* 
get_or_create_area(GArray * areas, int blob_id, int phase, coord_t coord) {
    area_t* area;
    for (int i=0; i<areas->len; i++) {
        area = &g_array_index(areas, area_t, i);
        if (area->blob_id == blob_id && area->phase == phase) {
            return area;
        }
    }
    area_t new_area = make_area(blob_id, phase, coord);
    g_array_append_val(areas, new_area);
    return &g_array_index(areas, area_t, areas->len-1);
}

GArray* find_areas(matrix_t* phases, matrix_t* blobs, int phase, int y) {
    GArray* areas = g_array_new(FALSE, FALSE, sizeof(area_t));
    region_t r = layer_region(phases, y);
    FOR_EACH_COORD(c, r) {
        if (get_voxel(phases, c) == phase) {
            area_t* area = get_or_create_area(areas, phase, get_voxel(blobs, c), c);
            area->bounding_box = extend_region(area->bounding_box, c);
        }        
    } END_FOR_EACH_COORD;
    return areas;
}
