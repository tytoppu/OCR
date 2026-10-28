#include "image.h"

/*
 * Ce fichier contient toute les fonction de chargement, 
 * et traitement elementaire de surface / image
 */




/* ========================================
                SDL_Surface
======================================== */



SDL_Surface* load_standar_picture (const char *path){
    SDL_Surface* picure = IMG_Load(path);
    if (!picure)
        errx(EXIT_FAILURE, "IMG_Load: %s",SDL_GetError());

    SDL_Surface* standar_picture = SDL_ConvertSurfaceFormat(picure,
            SDL_PIXELFORMAT_ARGB8888, 0);
    if (!standar_picture)
        errx(EXIT_FAILURE, "SDL_ConvertSurfaceFormat: %s",SDL_GetError());

    return standar_picture;
}

void save_picture(SDL_Surface* surface, const char *path){
    if (SDL_SaveBMP(surface, path) != 0)
        errx(EXIT_FAILURE, "SDL_SaveBMP: %s", SDL_GetError());
    //SDL_FreeSurface(surface);
}



/* ========================================
            struct simple_surface
======================================== */

struct simple_surface* init_simple_surface(int w, int h){
    struct simple_surface new = malloc(sizeof(struct simple_surface));
    if(!new) errx(EXIT_FAILURE, "malloc in init_simple_surface");

    new->w = w;
    new->h = h;
    new->gray = malloc(sizeof(Uint8) * w * h);

    return new;
}


SDL_Surface* simple_surface_to_surface(struct simple_surface* simple){
    SDL_Surface* new = SDL_CreateRGBSurfaceWithFormat(0, simple->w, simple->h,
            32, SDL_PIXELFORMAT_ARGB8888);
    return new;
}



struct simple_surface make_chunck(struct simple_surface simple, int x1, int y1, int x2, int y2){
    if (x1 > x2) {
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }
    if (y1 > y2) {
        int temp = y1;
        y1 = y2;
        y2 = temp;
    }

    if (x1 < 0 || x2 >= simple->w || y1 < 0 || y2 >= simple->h)
        errx(EXIT_FAILURE, "save_chunck : Input error, invalid x2 or y2");

    struct simple_surface chunck = init_simple_surface(x2 - x1, y2 - y1);
    if (!chunck) 
        errx(EXIT_FAILURE, "init_simple_surface");

    for (int x = x1; x <= x2; x++){
        for (int y = y1; y <= y2; y++){
        chunck->gray[]
        }
    }

    return chunck;
}

SDL_Surface* rotate_picture(SDL_Surface* surface, double angle, Uint8 font){
    double theta = angle * M_PI / 180.0;
    double c = cos(theta);
    double s = sin(theta);

    SDL_PixelFormat* form = surface->format;
    int w = surface->w;
    int h = surface->h;
    double cx = w/2.0;
    double cy = h/2.0;
    SDL_Surface* new = SDL_CreateRGBSurfaceWithFormat(0, w, h,
            form->BitsPerPixel, form->format);

    int bpp = new->format->BytesPerPixel;
    int pitch = new->pitch;

    Uint32 pixel;
    int y = 0;
    int x = 0;
    int x_s;
    int y_s;
    while(y < h){
        x = 0;
        while(x < w){

            x_s = (int)round(c * (x - cx) + s * (y - cy) + cx);
            y_s = (int)round(-s * (x - cx) + c * (y - cy) + cy);

            if (x_s >= 0 && x_s < w && y_s >= 0 && y_s < h)
                memcpy(&pixel, 
                        (Uint8 *) surface->pixels + y_s * pitch + x_s * bpp, 
                        bpp);
            else 
                pixel = SDL_MapRGB(form, font, font, font);
            memcpy((Uint8 *) new->pixels + y * pitch + x * bpp, &pixel, bpp);

            x++;
        }
        y++;
    }
    //SDL_FreeSurface(surface);
    return new;

}


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

