#include "cell_detection.h"

#include <stdlib.h>


//Construit un tableau a deux dimensions contenant
//les coordonnees de chaque case de la grille.
/*
 * grid    : coordonnees de la grille detectee.
 * rows    : nombre de lignes.
 * columns : nombre de colonnes.
 * cells   : adresse du tableau a creer.
 *
 * Return 1 succes, 0 erreur.
 */
int build_cells(const GridCoordinates *grid,
                int rows,
                int columns,
                CellCoordinates ***cells)
{
    	int x_left;
    	int x_right;
    	int y_top;
    	int y_bottom;

    	CellCoordinates **result;

    	if (cells == NULL)
        	return 0;

    	*cells = NULL;

    	if (grid == NULL || rows <= 0 || columns <= 0)
        	return 0;

    // Recup des limites de la grille 
    	x_left = grid->bounds.top_left.x;
    	x_right = grid->bounds.top_right.x;
    	y_top = grid->bounds.top_left.y;
    	y_bottom = grid->bounds.bottom_left.y;

    	if (x_right <= x_left || y_bottom <= y_top)
        	return 0;

    /* Alloc du tableau de lignes. */
    	result = malloc((size_t)rows * sizeof(*result));

    	if (result == NULL)
        	return 0;

    /* Alloc colonnes pour chaque ligne */
    	for (int row = 0; row < rows; row++) {
        	result[row] = malloc((size_t)columns * sizeof(*result[row]));

        	if (result[row] == NULL) {
            		for (int i = 0; i < row; i++)
                		free(result[i]);

            		free(result);
            	return 0;
        	}
    	}

// Calcul des coordonnes de chaque case 
    	for (int row = 0; row < rows; row++) {
        	int y1 = y_top+ (int)(((long long)(y_bottom - y_top) * row) / rows);

        	int y2 = y_top+ (int)(((long long)(y_bottom - y_top) * (row + 1)) / rows);

        	for (int column = 0; column < columns; column++) {
            		int x1 = x_left
                		+ (int)(((long long)(x_right - x_left) * column)/ columns);

            		int x2 = x_left
                		+ (int)(((long long)(x_right - x_left) * (column + 1))/ columns);

            		CellCoordinates *cell = &result[row][column];

            		cell->row = row;
            		cell->column = column;

            		cell->bounds.top_left.x = x1;
            		cell->bounds.top_left.y = y1;

            		cell->bounds.top_right.x = x2;
            		cell->bounds.top_right.y = y1;

            		cell->bounds.bottom_right.x = x2;
            		cell->bounds.bottom_right.y = y2;

            		cell->bounds.bottom_left.x = x1;
            		cell->bounds.bottom_left.y = y2;
        	}
    	}

    	*cells = result;

    	return 1;
}

// Libere la memoire utilisee par le tableau.

//cells : tableau a liberer.
// rows  : nombre de lignes allouees.

void free_cells(CellCoordinates **cells, int rows)
{
    if (cells == NULL)
        return;

    for (int row = 0; row < rows; row++)
        free(cells[row]);

    free(cells);
}
