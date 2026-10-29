#ifndef SIMPLE_SURFACE_H
#define SIMPLE_SURFACE_H

#include <SDL2/SDL.h>

struct simple_surface {      // suppose que l'image a été grayscalée
    int w;
    int h;
    Uint8 *gray;
};

#endif