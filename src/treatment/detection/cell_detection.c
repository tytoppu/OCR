#include "cell_detection.h"
#include "bitmap.h"
#include <err.h>
#include <stdlib.h>



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

void correlation(double* array, int size, double** output_correlation){
    double* array_correlation = calloc(size, sizeof(double));
    if (!array_correlation) errx(EXIT_FAILURE, "correlation : calloc failed");
    for(int k = 0; k < size; k++){
        double sum_R = 0.0;
        for (int i = 0; i < size-k; i++){
            sum_R += array[i] * array[i + k];
        }
        array_correlation[k] = sum_R / (size - k);  // normalization
    }
    *output_correlation = array_correlation;
}


int cell_size_detector(struct simple_surface* simple, Uint8 anchor, char axis){
    int imp;
    if (axis == 'x') imp = simple->w;
    else if (axis == 'y') imp = simple->h;
    else
        errx(EXIT_FAILURE, "cell_size_detector : wrong axis input");


    double* width_histogram = calloc(imp, sizeof(double));
    if (!width_histogram)
        errx(EXIT_FAILURE, "cell_width_detector : calloc failed");
    for(int y = 0; y<simple->h; y++){
        for(int x = 0; x<simple->w; x++){
            if (simple->gray[y*simple->w + x] == anchor) {
                if (axis == 'x') width_histogram[x] ++;
                else width_histogram[y] ++;
            }
        }
    }

    double mean = 0.0;
    for(int i = 0; i < imp; i++) mean += width_histogram[i];
    mean = mean / imp;
    for(int i = 0; i < imp; i++) width_histogram[i] -= mean;

    double* corr = NULL;
    correlation(width_histogram, imp, &corr);

    free(width_histogram);
    double corr0 = corr[0];
    for (int i = 0; i < imp; i++) corr[i] /= corr0;

    int k = 0;
    int limit = imp / 2;
    while(k < limit && corr[k] >= 0.1)             // 1st phase
        k++;

    int max_glob = k;
    for(int i = k; i < limit; i++)                     // max_glob
        if (corr[i] > corr[max_glob]) max_glob = i; 


    for(int i = k+1; i < limit-1; i++){                         // max_glob
        if (corr[i] >= corr[max_glob] * 0.7 && corr[i] >= corr[i+1]
                && corr[i] >= corr[i-1]) {
            free(corr);
            return i;
        }
    }
                   
    free(corr);
    errx(EXIT_FAILURE, "cell_width_detector : no reliable cell width found");
}



void build_cells(struct simple_surface* simple, struct cell_grid** cell,
        Uint8 anchor){

    if (!simple)
        errx(EXIT_FAILURE, "build_cells : wrong input : simple");

    if (anchor != 0 && anchor != 255)
        errx(EXIT_FAILURE, "build_cells : wrong input : anchor");

    int w_cell = cell_size_detector(simple, anchor, 'x');
    int h_cell = cell_size_detector(simple, anchor, 'y');
    int nb_columns = simple->w / w_cell;
    int nb_rows = simple->h / h_cell;
    //int nb_columns = (int)round((double) simple->w / w_cell);
    //int nb_rows = (int)round((double) simple->h / h_cell);

    struct cell_grid* grid = init_cell_grid(nb_columns, nb_rows);
    struct simple_surface*** new =
        malloc(nb_rows * sizeof(struct simple_surface**));

    for(int y = 0; y<nb_rows; y++){
        new[y] = malloc(nb_columns * sizeof(struct simple_surface*));
        for(int x = 0; x<nb_columns; x++){
            new[y][x] = make_chunk(simple, x*w_cell, y*h_cell,
                    (x+1)*w_cell, (y+1)*h_cell);
        }
    }
    grid->simple = new;
    *cell = grid;
}