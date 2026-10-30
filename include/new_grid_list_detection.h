#ifndef NEW_GRID_LIST_DETECTION_C
#define NEW_GRID_LIST_DETECTION_C

#include "detection_data.h"
#include "simple_surface.h"

#include "bitmap.h"

void where_is_anchor(struct simple_surface* simple, int* col, int* row,
        Uint8 anchor);
struct simple_surface* scrap(struct simple_surface* simple, int* col,
        int* row, int* l, int* r, int* t, int* b);
struct simple_surface* mini_scrap(struct simple_surface* simple,
        int x1, int y1, int x2, int y2, Uint8 anchor);
void max_empty_range (int* array, int size, double threshold,
        int* len_out, int* x_out);
void detect_grid_and_list(struct simple_surface* simple,
        struct simple_surface** grid, struct simple_surface** list,
        Uint8 anchor);



#endif /* NEW_GRID_LIST_DETECTION_C */
