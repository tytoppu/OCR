#ifndef SIMPLE_SURFACE_H
#define SIMPLE_SURFACE_H

#include <SDL2/SDL.h>

struct simple_surface {      // assumes the image has been grayscaled
    int w;
    int h;
    Uint8 *gray;
};

struct cell_grid {
    int w;
    int h;
    struct simple_surface*** simple;   // cells[row][col]
};

struct word_list{
    struct letter_list* letters;
    struct simple_surface* word;
    struct word_list* next;
};

struct letter_list{
    struct simple_surface* letter;
    struct letter_list* next;
};

#endif
