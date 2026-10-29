#ifndef CELL_DETECTION_H
#define CELL_DETECTION_H

#include "coordinates.h"
#include "detection_data.h"


void build_cells(const GridCoordinates *grid,
                CellCoordinates ***cells);


void free_cells(CellCoordinates **cells, int rows);


#endif /* CELL_DETECTION_H */
