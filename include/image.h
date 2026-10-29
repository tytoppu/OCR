#ifndef IMAGE_H
#define IMAGE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>
#include <err.h>
#include <math.h>

#include "cell_detection.h"
#include "coordinates.h"
#include "detection_data.h"
#include "detection_manager.h"
#include "dominant_color.h"
#include "grid_detection.h"
#include "letter_detection.h"
#include "word_list_detection.h"

#define CONTIGUITY 0.05

struct simple_surface{      // gray suppose que l'image a etait grayscaled
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
                bitmap.c
======================================== */

// SDL_Surface

SDL_Surface* load_standar_picture (const char *path);
void save_picture(SDL_Surface* surface, const char *path);
void save_simple(struct simple_surface* simple, const char *path);


// struct simple_surface

struct simple_surface* init_simple_surface(int w, int h);
SDL_Surface* simple_surface_to_surface(struct simple_surface* simple);
void destroy_simple_surface(struct simple_surface* simple);


// traitement elementaire

struct simple_surface* make_chunck(struct simple_surface* simple, int x1, int y1, int x2, int y2);
struct simple_surface* rotate_picture(struct simple_surface* simple, double angle, Uint8 font);


// pixel

Uint32 get_pixel(SDL_Surface *surface, int x, int y);
void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel);




/* ========================================
                filter.c
======================================== */

// auxiliaire

void contigu (struct simple_surface* simple, int histogram[256], int* lower_pix, int* highter_pix);
void insert_array(Uint8 array[9], int* size, Uint8 elem);


// grayscale

Uint8 single_grayscale(Uint8 r, Uint8 g, Uint8 b);
struct simple_surface* start_grayscale(SDL_Surface* surface);


// median filter

Uint8 single_median_filter(struct simple_surface* simple, int x, int y);
struct simple_surface* median_filter(struct simple_surface* simple, int histogram[256]);


// contrast

Uint8 single_contrast(Uint8 gray, Uint8 min, Uint8 max);
struct simple_surface* contrast(struct simple_surface* simple, int histogram[256]);


// black_and_white

Uint8 single_black_and_white(Uint8 gray);
struct simple_surface* black_and_white(struct simple_surface* simple);


// rotate image

int evaluate_rotation(struct simple_surface* simple, double angle);


// MAIN : normalizer

struct simple_surface* normalizer(SDL_Surface *surface);



/* ========================================
                image.c
======================================== */


void pretreatment(SDL_Surface* surface);
char* naming_point_bnp(const char* prefix, int x, int y);


#endif
