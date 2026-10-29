#ifndef NEW_GRID_LIST_DETECTION_C
#define NEW_GRID_LIST_DETECTION_C

#include "detection_data.h"
#include "simple_surface.h"

#include "bitmap.h"

int is_colors(struct simple_surface* simple,int x, int y,int colors);
void dectet_grid_and_list(struct simple_surface* simple, struct simple_surface* grid, struct simple_surface* list, int colors);


#endif /* NEW_GRID_LIST_DETECTION_C */
