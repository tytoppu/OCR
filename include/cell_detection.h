#ifndef CELL_DETECTION_H
#define CELL_DETECTION_H

#include "coordinates.h"
#include "detection_data.h"
#include "simple_surface.h"


void correlation(double* array, int size, double** output_correlation);
//int cell_width_detector(struct simple_surface* simple, Uint8 anchor);
//int cell_height_detector(struct simple_surface* simple, Uint8 anchor);
int cell_size_detector(struct simple_surface* simple, Uint8 anchor, char axis);

void build_cells(struct simple_surface* simple, struct cell_grid** cell,
        Uint8 anchor);

//void free_cells(CellCoordinates **cells, int rows);


#endif /* CELL_DETECTION_H */
