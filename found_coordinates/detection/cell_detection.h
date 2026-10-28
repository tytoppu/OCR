#ifndef CELL_DETECTION_H
#define CELL_DETECTION_H

#include "../structures/coordinates.h"
#include "../structures/detection_data.h"


int build_cells(const GridCoordinates *grid,
                int rows,
                int columns,
                CellCoordinates ***cells);


void free_cells(CellCoordinates **cells, int rows);

#endif /* CELL_DETECTION_H */
