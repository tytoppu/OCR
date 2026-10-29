#ifndef BITMAP_H
#define BITMAP_H

#include "simple_surface.h"

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


#endif /* BITMAP_H */