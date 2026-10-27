#include "image.h"
#include <err.h>

SDL_Surface *load_image(const char *path)
{
    SDL_Surface* picure = IMG_Load(path);
    if (!picure)
        errx(EXIT_FAILURE, "IMG_Load: %s",SDL_GetError());

    return picure;
}

void save_image(SDL_Surface *image, const char *path)
{
    if (SDL_SaveBMP(image, path) != 0)
        errx(EXIT_FAILURE, "SDL_SaveBMP: %s", SDL_GetError());
    //SDL_FreeSurface(image);
}
