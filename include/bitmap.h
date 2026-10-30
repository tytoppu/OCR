#ifndef BITMAP_H
#define BITMAP_H

#include "simple_surface.h"

// SDL_Surface

SDL_Surface* load_standard_picture (const char *path);
void save_picture(SDL_Surface* surface, const char *path);
void save_simple(struct simple_surface* simple, const char *path);


// struct simple_surface

//init
struct simple_surface* init_simple_surface(int w, int h);
struct cell_grid* init_cell_grid(int w, int h);
struct word_list* init_word_list(void);
struct letter_list* init_letter_list(void);
//destroy
void destroy_simple_surface(struct simple_surface* simple);
void destroy_cell_grid(struct cell_grid* grid);
void destroy_letter_list(struct letter_list* letter_l);
void destroy_word_list(struct word_list* word_l);
//other
SDL_Surface* simple_surface_to_surface(struct simple_surface* simple);
void append_word_list(struct word_list** word_l, struct word_list* new);
void append_letter_list(struct letter_list** letter_l, struct letter_list* new);



// basic processing

struct simple_surface* make_chunk(struct simple_surface* simple,
        int x1, int y1, int x2, int y2);
struct simple_surface* rotate_picture(struct simple_surface* simple,
        double angle, Uint8 background);


// pixel

Uint32 get_pixel(SDL_Surface *surface, int x, int y);
void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel);


#endif /* BITMAP_H */











