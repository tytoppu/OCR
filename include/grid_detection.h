#ifndef GRID_DETECTION_H
#define GRID_DETECTION_H

#include "coordinates.h"
#include "simple_surface.h"




void detect_grid_bounds(struct simple_surface* simple,
                       GridCoordinates *grid,
		       int colors
		       );

void detect_grid_dimensions(
    struct simple_surface* simple,
    GridCoordinates *grid,
    int colors
);


#endif /* GRID_DETECTION_H */
