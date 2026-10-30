#ifndef FILTER_H
#define FILTER_H

#include "simple_surface.h"


#define CONTIGUITY 0.05

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
struct simple_surface* black_and_white(struct simple_surface* simple, Uint8* ancre);


// rotate image

int evaluate_rotation(struct simple_surface* simple, double angle);


// MAIN : normalizer

struct simple_surface* normalizer(SDL_Surface *surface, Uint8* ancre);

#endif  /* FILTER_H */