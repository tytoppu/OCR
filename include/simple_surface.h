#ifndef SIMPLE_SURFACE_H
#define SIMPLE_SURFACE_H

#include <SDL2/SDL.h>

struct simple_surface {      // suppose que l'image a été grayscalée
    int w;
    int h;
    Uint8 *gray;
};

struct cell_grid {
    int rows;
    int cols;
    struct simple_surface*** simple;   // cells[row][col]
};

struct word_liste{
    struct letre_list* letre;
    struct simple_surface* word;
    struct word_liste* next;
};

struct letre_list{
    struct simple_surface* letre;
    struct letre_list* next;
};

#endif
