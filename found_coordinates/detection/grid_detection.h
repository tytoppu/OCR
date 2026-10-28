#ifndef GRID_DETECTION_H
#define GRID_DETECTION_H

#include "../structures/coordinates.h"





int detect_grid_bounds(const unsigned char *image,
                       int width,
                       int height,
                       int stride,
                       GridCoordinates *grid);

int detect_grid_dimensions(
    const unsigned char *image,
    int width,
    int height,
    int stride,
    GridCoordinates *grid
);


#endif /* GRID_DETECTION_H */
