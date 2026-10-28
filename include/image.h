#ifndef IMAGE_H
#define IMAGE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stddef.h>
#include <err.h>
#include <math.h>

//#define WINDOW_HEIGHT 600
//#define WINDOW_WIDTH 800


struct simple_surface{
    int w;
    int h;
    Uint8* gray;
};


//      bitmap.c

SDL_Surface *load_image(const char *path);
void save_image(SDL_Surface *surface, const char *path);
SDL_Surface* make_chunck(SDL_Surface* surface, int x1, int y1, int x2, int y2);
SDL_Surface* rotate_picture(SDL_Surface* surface, double angle, Uint8 font);

Uint32 get_pixel(SDL_Surface *surface, int x, int y);
void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel);


/*
 * Charge un BMP non compresse (24 ou 32 bits), le convertit en niveaux
 * de gris, le redimensionne en largeur x hauteur, puis normalise chaque
 * pixel dans [0.0, 1.0].
 *
 * `destination` doit contenir au moins largeur * hauteur doubles.
 *
 * Retourne 1 en cas de succes, 0 en cas d'erreur.
 */
int charger_image_bmp_double(const char *chemin,
                             double *destination,
                             size_t largeur,
                             size_t hauteur);



//      filter.c

//auxiliaire
void contigu (SDL_Surface* surface, int histogram[256], int* lower_pix, int* highter_pix);
Uint8 GrayAt(SDL_PixelFormat *form, Uint8* p);
void insert_array(Uint8 array[9], int* size, Uint8 elem);

//grayscale
Uint8 single_grayscale(Uint8 r, Uint8 g, Uint8 b);
void grayscale(SDL_Surface* surface);

//median filter
Uint8 single_median_filter(SDL_Surface* surface, int x, int y);
SDL_Surface* median_filter(SDL_Surface *surface, int histogram[256]);

//contrast
Uint8 single_contrast(Uint8 gray, Uint8 min, Uint8 max);
void contrast(SDL_Surface* surface, int histogram[256]);

// black and white
Uint8 black_and_white(Uint8 gray);

// MAIN : normalizer
SDL_Surface* normalizer(SDL_Surface *surface);


#endif
