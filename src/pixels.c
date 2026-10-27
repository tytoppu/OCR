#include "image.h"
#include <err.h>

Uint32 get_pixel(SDL_Surface *surface, int x, int y)
{
    if(surface->format->BytesPerPixel != 4) errx(EXIT_FAILURE, "The Bytes Per Pixel isn't 4");
    int size = y * surface->w + x;
    Uint32* pixel = (Uint32*)(surface->pixels);
    return *(pixel + size);
    //en gros c un jeux de tail de memoir entre pitch pour les y et sizeof(pixel) pour les x, on faite genre surfacce->pixe[y*pitch + x*sizeof(pixel)] et on tomber sur le pixel quon veu
}

void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    if(surface->format->BytesPerPixel != 4) errx(EXIT_FAILURE, "The Bytes Per Pixel isn't 4");
    int size = y * surface->w + x;
    Uint32* the_pixel = (Uint32*)(surface->pixels);
    *(the_pixel + size) = pixel;
}

