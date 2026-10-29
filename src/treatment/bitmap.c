#include "bitmap.h"
#include <SDL2/SDL_image.h>
#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

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
    SDL_FreeSurface(picure);
    if (!standar_picture)
        errx(EXIT_FAILURE, "SDL_ConvertSurfaceFormat: %s",SDL_GetError());

    return standar_picture;
}

void save_picture(SDL_Surface* surface, const char *path){
    if (SDL_SaveBMP(surface, path) != 0)
        errx(EXIT_FAILURE, "SDL_SaveBMP: %s", SDL_GetError());
    //SDL_FreeSurface(surface);
}

void save_simple(struct simple_surface* simple, const char *path){
    SDL_Surface* saving = simple_surface_to_surface(simple);
    save_picture(saving, path);
    SDL_FreeSurface(saving);
}


/* ========================================
            struct simple_surface
======================================== */

struct simple_surface* init_simple_surface(int w, int h){
    struct simple_surface* new = malloc(sizeof(struct simple_surface));
    if(!new) errx(EXIT_FAILURE, "malloc in init_simple_surface");

    new->w = w;
    new->h = h;
    new->gray = malloc(sizeof(Uint8) * w * h);
    if(!new->gray) errx(EXIT_FAILURE, "malloc in init_simple_surface");

    return new;
}


SDL_Surface* simple_surface_to_surface(struct simple_surface* simple){
    SDL_Surface* new = SDL_CreateRGBSurfaceWithFormat(0, simple->w, simple->h,
            32, SDL_PIXELFORMAT_ARGB8888);
    if(!new)
        errx(EXIT_FAILURE, "malloc in simple_surface_to_surface");

    Uint32 pixel;
    Uint8 gray;
    for(int y = 0; y < simple->h ;y++) {
        for(int x = 0; x < simple->w ;x++) {
            gray = simple->gray[y*simple->w + x];
            //printf("%i\n",gray);
            pixel = SDL_MapRGB(new->format, gray, gray, gray);
            //printf("%i\n",pixel);
            set_pixel(new, x, y, pixel);
        }
    }
    //save_picture(new, "firstly.bmp");
    return new;
}

void destroy_simple_surface(struct simple_surface* simple){
    if (!simple) return;

    if (simple->gray) free(simple->gray);
    free(simple);
}

/* ========================================
        traitement elementaire
======================================== */

struct simple_surface* make_chunck(struct simple_surface* simple, int x1, int y1, int x2, int y2){
    /*
     * renvoi une struct simple_surface 
     * contenent les pixel du chunck de (x1,y1) exclu a (x2y2) exclu
     */

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

    if (x1 < 0 || x2 > simple->w || y1 < 0 || y2 > simple->h || x2-x1<2 || y2-y1<2)
        errx(EXIT_FAILURE, "make_chunck : Input error, invalid x or y");

    struct simple_surface* chunck = init_simple_surface(x2 - x1 - 1, y2 - y1 - 1);
    if (!chunck) 
        errx(EXIT_FAILURE, "init_simple_surface");

    for (int x = 0; x < chunck->w; x++){
        for (int y = 0; y < chunck->h; y++)
            chunck->gray[y*chunck->w + x] = simple->gray[(y+y1+1)*simple->w + (x+x1+1)];
    }

    return chunck;
}

struct simple_surface* rotate_picture(struct simple_surface* simple, double angle, Uint8 font){
    /*
     * renvoi un struct simple_surface 
     * qui est le parametre simple mais rotate. rien n'a etait free
     */

    double theta = angle * M_PI / 180.0;
    double c = cos(theta);
    double s = sin(theta);


    int new_w = (int)round(fabs(simple->w * c) + fabs(simple->h * s)) + 1;
    int new_h = (int)round(fabs(simple->w * s) + fabs(simple->h * c)) + 1;

    struct simple_surface* rotate = init_simple_surface(new_w, new_h);
    if (!rotate) 
        errx(EXIT_FAILURE, "init_simple_surface failure in rotate_picture");

    double cx = simple->w/2.0;
    double cy = simple->h/2.0;
    double cnew_x = new_w/2.0;
    double cnew_y = new_h/2.0;

    Uint8 pixel;
    int y = 0;
    int x = 0;
    int x_s;
    int y_s;
    while(y < new_h){
        x = 0;
        while(x < new_w){

            x_s = (int)round(c * (x - cnew_x) + s * (y - cnew_y) + cx);
            y_s = (int)round(-s * (x - cnew_x) + c * (y - cnew_y) + cy);

            if (x_s >= 0 && x_s < simple->w && y_s >= 0 && y_s < simple->h)
                pixel = simple->gray[y_s * simple->w + x_s];
            else 
                pixel = font;
            rotate->gray[y * new_w + x] = pixel;

            x++;
        }
        y++;
    }
    return rotate;
}


/* ========================================
                pixel
======================================== */



Uint32 get_pixel(SDL_Surface *surface, int x, int y){
    /*
     * renvoi la valeur Uint32 d'un pixel
     */

    if(!surface) errx(EXIT_FAILURE, "get_pixel : no surface");
    Uint8* p = (Uint8*)(surface->pixels) 
            + y*surface->pitch 
            + x*surface->format->BytesPerPixel;
    Uint32 pixel;
    memcpy(&pixel,p, surface->format->BytesPerPixel);
    return pixel;
}


void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel){
    /*
     * modifi directement un pixel dans une surface
     */

    if(!surface) errx(EXIT_FAILURE, "set_pixel : no surface");
    Uint8* p = (Uint8*)(surface->pixels) 
            + y*surface->pitch 
            + x*surface->format->BytesPerPixel;
    memcpy(p, &pixel, surface->format->BytesPerPixel);
}
