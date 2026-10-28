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
#define CONTIGUITY 800

struct simple_surface{
    int w;
    int h;
    Uint8* gray;
};


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


/* ========================================
                filter.c
======================================== */

// auxiliaire

//Uint8 GrayAt(SDL_PixelFormat *form, Uint8* p);
void contigu (struct simple_surface simple, int histogram[256], int* lower_pix, int* highter_pix);
void insert_array(Uint8 array[9], int* size, Uint8 elem);


// grayscale

Uint8 single_grayscale(Uint8 r, Uint8 g, Uint8 b);
struct simple_surface* start_grayscale(SDL_Surface* surface);


// median filter

Uint8 single_median_filter(struct simple_surface simple, int x, int y);
struct simple_surface* median_filter(struct simple_surface* simple, int histogram[256]);


// contrast

Uint8 single_contrast(Uint8 gray, Uint8 min, Uint8 max);
struct simple_surface* contrast(struct simple_surface* simple, int histogram[256]);


// black_and_white

Uint8 single_Otsu(Uint8 gray, int array;


// rotate image

int evaluate_rotation(struct simple_surface simple, double angle);


// MAIN : normalizer

struct simple_surface* normalizer(SDL_Surface *surface);



/* ========================================
                bitmap.c
======================================== */

// SDL_Surface

SDL_Surface* load_standar_picture (const char *path);
void save_picture(SDL_Surface* surface, const char *path);


// struct simple_surface

struct simple_surface* init_simple_surface(int w, int h);
SDL_Surface* simple_surface_to_surface(struct simple_surface* simple);
void destroy_simple_surface(struct simple_surface simple);


// traitement elementaire

struct simple_surface make_chunck(struct simple_surface simple, int x1, int y1, int x2, int y2);
struct simple_surface* rotate_picture(struct simple_surface* simple, double angle, Uint8 font);


// pixel

Uint32 get_pixel(SDL_Surface *surface, int x, int y);
void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel);



#endif
