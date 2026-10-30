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

void corelation(double* array, int size, double** output_correlation){
    double* array_correlation = calloc(size, sizeof(double));
    if (!array_correlation) errx(EXIT_FAILURE, "corelation : calloc failed");
    for(int k = 0; k < size; k++){
        double sum_R = 0.0;
        for (int i = 0; i < size-k; i++){
            sum_R += array[i] * array[i + k];
        }
        array_correlation[k] = sum_R / (size - k);  // normalisation
    }
    *output_correlation = array_correlation;
}


int cell_size_detector(struct simple_surface* simple, Uint8 ancre, char axe){
    int imp;
    if (axe == 'x') imp = simple->w;
    else if (axe == 'y') imp = simple->h;
    else
        errx(EXIT_FAILURE, "cell_size_detector : wrong axe input");


    double* width_histogramme = calloc(imp, sizeof(double));
    if (!width_histogramme) errx(EXIT_FAILURE, "cell_width_detector : calloc failed");
    for(int y = 0; y<simple->h; y++){
        for(int x = 0; x<simple->w; x++){
            if (simple->gray[y*simple->w + x] == ancre) {
                if (axe == 'x') width_histogramme[x] ++;
                else width_histogramme[y] ++;
            }
        }
    }

    double mean = 0.0;
    for(int i = 0; i < imp; i++) mean += width_histogramme[i];
    mean = mean / imp;
    for(int i = 0; i < imp; i++) width_histogramme[i] -= mean;

    double* corr = NULL;
    corelation(width_histogramme, imp, &corr);

    free(width_histogramme);
    double corr0 = corr[0];
    for (int i = 0; i < imp; i++) corr[i] /= corr0;

    int k = 0;
    int limite = imp / 2;
    while(k < limite && corr[k] >= 0.1)             // 1er phase
        k++;

    int max_glob = k;
    for(int i = k; i < limite; i++)                     // max_glob
        if (corr[i] > corr[max_glob]) max_glob = i; 


    for(int i = k+1; i < limite-1; i++){                         // max_glob
        if (corr[i] >= corr[max_glob] * 0.7 && corr[i] >= corr[i+1] && corr[i] >= corr[i-1]) {
            free(corr);
            return i;
        }
    }
                   
    free(corr);
    err(EXIT_FAILURE, "cell_width_detector : no fiable widh cell found");
}



void build_cells(struct simple_surface* simple, struct cell_grid** cell, Uint8 ancre){

    if (!simple)
        errx(EXIT_FAILURE, "build_cells : wrong input : simple");

    if (ancre != 0 && ancre != 255)
        errx(EXIT_FAILURE, "build_cells : wrong input : ancre");

    int w_cell = cell_size_detector(simple, ancre, 'x');
    int h_cell = cell_size_detector(simple, ancre, 'y');
    int widerness = simple->w / w_cell;
    int highterness = simple->h / h_cell;
    //int widerness = (int)round((double) simple->w / w_cell);
    //int highterness = (int)round((double) simple->h / h_cell);

    struct cell_grid* grid = init_cell_grid(widerness, highterness);
    struct simple_surface*** new = malloc(highterness * sizeof(struct simple_surface**));

    for(int y = 0; y<highterness; y++){
        new[y] = malloc(widerness * sizeof(struct simple_surface*));
        for(int x = 0; x<widerness; x++){
            new[y][x] = make_chunck(simple, x*w_cell, y*h_cell, (x+1)*w_cell, (y+1)*h_cell);
        }
    }
    grid->simple = new;
    *cell = grid;
}