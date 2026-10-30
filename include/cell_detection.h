#ifndef CELL_DETECTION_H
#define CELL_DETECTION_H

#include "coordinates.h"
#include "detection_data.h"
#include "simple_surface.h"


void corelation(double* array, int size, double** output_correlation);
//int cell_width_detector(struct simple_surface* simple, Uint8 ancre);
//int cell_height_detector(struct simple_surface* simple, Uint8 ancre);
int cell_size_detector(struct simple_surface* simple, Uint8 ancre, char axe);

struct simple_surface*** build_cells(struct simple_surface* simple, Uint8 ancre);

//void free_cells(CellCoordinates **cells, int rows);


#endif /* CELL_DETECTION_H */
