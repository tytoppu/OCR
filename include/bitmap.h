#ifndef BITMAP_H
#define BITMAP_H

#include "simple_surface.h"

// SDL_Surface

SDL_Surface* load_standar_picture (const char *path);
void save_picture(SDL_Surface* surface, const char *path);
void save_simple(struct simple_surface* simple, const char *path);


// struct simple_surface

//init
struct simple_surface* init_simple_surface(int w, int h);
struct cell_grid* init_cell_grid(int w, int h);
struct word_liste* init_word_liste(void);
struct letre_list* init_letre_list(void);
//destroy
void destroy_simple_surface(struct simple_surface* simple);
void destroy_cell_grid(struct cell_grid* grid);
void destroy_letre_list(struct letre_list* letre_l);
void destroy_word_liste(struct word_liste* word_l);
//other
SDL_Surface* simple_surface_to_surface(struct simple_surface* simple);
void append_word_liste(struct word_liste** word_l, struct word_liste* new);
void append_letre_list(struct letre_list** letre_l, struct letre_list* new);



// traitement elementaire

struct simple_surface* make_chunck(struct simple_surface* simple, int x1, int y1, int x2, int y2);
struct simple_surface* rotate_picture(struct simple_surface* simple, double angle, Uint8 font);


// pixel

Uint32 get_pixel(SDL_Surface *surface, int x, int y);
void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel);


#endif /* BITMAP_H */











