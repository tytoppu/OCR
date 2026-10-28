#include <stdio.h>

#include "structures/coordinates.h"
#include "structures/detection_data.h"
#include "detection/cell_detection.h"

/*
  Teste la fonction build_cells().
 
  Retourne 1 si tous les tests reussissent,
  0 si un test echoue.
 */
int test_cell_detection(void)
{
    GridCoordinates grid;
    CellCoordinates **cells = NULL;

    int rows = 3;
    int columns = 3;

    printf("\n=== TEST DE CELL_DETECTION ===\n");
    grid.bounds.top_left.x = 10;
    grid.bounds.top_left.y = 20;

    grid.bounds.top_right.x = 100;
    grid.bounds.top_right.y = 20;

    grid.bounds.bottom_left.x = 10;
    grid.bounds.bottom_left.y = 80;

    grid.bounds.bottom_right.x = 100;
    grid.bounds.bottom_right.y = 80;

    printf("Grille de test : 3 lignes x 3 colonnes\n");
    printf("Coin superieur gauche : (10, 20)\n");
    printf("Coin inferieur droit  : (100, 80)\n");

    if (!build_cells(&grid, rows, columns, &cells)) {
        printf("ERREUR : impossible de creer les cases.\n");
        return 0;
    }

    printf("\nCreation des cases reussie !\n");

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            CellCoordinates *cell = &cells[row][column];

            printf(
                "Case [%d][%d] : "
                "haut-gauche (%d, %d), "
                "bas-droite (%d, %d)\n",
                row,
                column,
                cell->bounds.top_left.x,
                cell->bounds.top_left.y,
                cell->bounds.bottom_right.x,
                cell->bounds.bottom_right.y
            );
        }
    }

    if (cells[0][0].bounds.top_left.x != 10 ||
        cells[0][0].bounds.top_left.y != 20) {
        printf("\nECHEC : coordonnees de la premiere case incorrectes.\n");
        free_cells(cells, rows);
        return 0;
    }

    if (cells[2][2].bounds.bottom_right.x != 100 ||
        cells[2][2].bounds.bottom_right.y != 80) {
        printf("\nECHEC : coordonnees de la derniere case incorrectes.\n");
        free_cells(cells, rows);
        return 0;
    }

    if (cells[1][1].row != 1 ||
        cells[1][1].column != 1) {
        printf("\nECHEC : indices de la case centrale incorrects.\n");
        free_cells(cells, rows);
        return 0;
    }

    printf("\nTous les tests ont reussi !\n");

    free_cells(cells, rows);

    return 1;
}





int main(void)
{
    return test_cell_detection() ? 0 : 1;
}
