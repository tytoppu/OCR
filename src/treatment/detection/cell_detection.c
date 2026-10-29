#include "cell_detection.h"




//Build a two-dimensional array containing
//the coordinates of each cell in the grid.
/*
* grid: coordinates of the detected grid.
* rows: number of rows.
* columns: number of columns.
* cells: address of the array to create.
*
* Returns 1 on success, 0 on error.
*/

#include <err.h>
#include <stdlib.h>
#include "cell_detection.h"

int build_cells(const GridCoordinates *grid, CellCoordinates ***cells)
{
    if (grid == NULL || cells == NULL ||
        grid->rows <= 0 || grid->columns <= 0 ||
        grid->cell_width <= 0 || grid->cell_height <= 0)
    {
        errx(EXIT_FAILURE, "build_cells : wrong input");
    }

    int rows = grid->rows;
    int columns = grid->columns;

    *cells = malloc(rows * sizeof(CellCoordinates *));
    if (*cells == NULL)
        errx(EXIT_FAILURE, "build_cells : malloc failed");

    for (int row = 0; row < rows; row++)
    {
        (*cells)[row] = malloc(columns * sizeof(CellCoordinates));

        if ((*cells)[row] == NULL)
        {
            for (int i = 0; i < row; i++)
                free((*cells)[i]);

            free(*cells);
            *cells = NULL;
            errx(EXIT_FAILURE, "build_cells : malloc failed on (*cells)[row]");
        }
    }

    int left = grid->bounds.top_left.x;
    int top = grid->bounds.top_left.y;

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < columns; col++)
        {
            CellCoordinates *cell = &(*cells)[row][col];

            cell->row = row;
            cell->column = col;

            cell->bounds.top_left.x =
                left + col * grid->cell_width;
            cell->bounds.top_left.y =
                top + row * grid->cell_height;

            cell->bounds.top_right.x =
                cell->bounds.top_left.x + grid->cell_width;
            cell->bounds.top_right.y =
                cell->bounds.top_left.y;

            cell->bounds.bottom_left.x =
                cell->bounds.top_left.x;
            cell->bounds.bottom_left.y =
                cell->bounds.top_left.y + grid->cell_height;

            cell->bounds.bottom_right.x =
                cell->bounds.top_right.x;
            cell->bounds.bottom_right.y =
                cell->bounds.bottom_left.y;
        }
    }

    return 1;
}








// Frees the memory used by the array.

// cells: array to free.
// rows: number of allocated rows.

void free_cells(CellCoordinates **cells, int rows)
{
    if (cells == NULL)
        return;

    for (int row = 0; row < rows; row++)
        free(cells[row]);

    free(cells);
}
