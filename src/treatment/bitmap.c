#include "bitmap.h"
#include <SDL2/SDL_image.h>
#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/*
 * This file contains all the loading functions
 * and the basic processing of surfaces / images
 */




/* ========================================
                SDL_Surface
======================================== */



SDL_Surface* load_standard_picture (const char *path){
    SDL_Surface* picture = IMG_Load(path);
    if (!picture)
        errx(EXIT_FAILURE, "IMG_Load: %s",SDL_GetError());

    SDL_Surface* standard_picture = SDL_ConvertSurfaceFormat(picture,
            SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(picture);
    if (!standard_picture)
        errx(EXIT_FAILURE, "SDL_ConvertSurfaceFormat: %s",SDL_GetError());

    return standard_picture;
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


// init


struct simple_surface* init_simple_surface(int w, int h){
    struct simple_surface* new = malloc(sizeof(struct simple_surface));
    if(!new) errx(EXIT_FAILURE, "malloc in init_simple_surface");

    new->w = w;
    new->h = h;
    new->gray = malloc(sizeof(Uint8) * w * h);
    if(!new->gray) errx(EXIT_FAILURE, "malloc in init_simple_surface");

    return new;
}

struct cell_grid* init_cell_grid(int w, int h){
    struct cell_grid* new = malloc(sizeof(struct cell_grid));
    if(!new) errx(EXIT_FAILURE, "malloc in init_cell_grid");

    new->w = w;
    new->h = h;
    new->simple = NULL;

    return new;

}

struct word_list* init_word_list(void){
    struct word_list* new = malloc(sizeof(struct word_list));
    if(!new) errx(EXIT_FAILURE, "malloc in init_word_list");

    new->letters = NULL;
    new->word = NULL;
    new->next = NULL;

    return new;
}

struct letter_list* init_letter_list(void){
    struct letter_list* new = malloc(sizeof(struct letter_list));
    if(!new) errx(EXIT_FAILURE, "malloc in init_letter_list");

    new->letter = NULL;
    new->next = NULL;

    return new;
}


// destroy


void destroy_simple_surface(struct simple_surface* simple){
    if (!simple) return;

    if (simple->gray) free(simple->gray);
    free(simple);
}

void destroy_cell_grid(struct cell_grid* grid){
    if (!grid) return;
    if (grid->simple){
        for (int y = 0; y<grid->h; y++){
            if (grid->simple[y]){
                for (int x = 0; x<grid->w; x++) 
                    if (grid->simple[y][x]) 
                        destroy_simple_surface(grid->simple[y][x]);
                free(grid->simple[y]);
            }
        }
        free(grid->simple);
    }
    free(grid);   
}

void destroy_letter_list(struct letter_list* letter_l){
    if(!letter_l) return;
    //errx(EXIT_FAILURE, "destroy_letter_list : free nothing");

    struct letter_list* current = letter_l;
    struct letter_list* prv = NULL;
    while (current){
        prv = current;
        current = current->next;

        destroy_simple_surface(prv->letter);
        free(prv);
        prv = NULL;

    }
}

void destroy_word_list(struct word_list* word_l){
    if(!word_l) return ;
    //errx(EXIT_FAILURE, "init_word_list : free nothing");

    struct word_list* current = word_l;
    struct word_list* prv = NULL;
    while (current){
        prv = current;
        current = current->next;

        destroy_letter_list(prv->letters);
        destroy_simple_surface(prv->word);
        free(prv);
        prv = NULL;

    }
}




// other


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

void append_word_list(struct word_list** word_l, struct word_list* new){
    if(!word_l) errx(EXIT_FAILURE, "append_word_list : word_l doesn't exist");
    
    if(!(*word_l)) {
        *word_l = new;
        return;
    }

    struct word_list* current = *word_l;
    while(current->next) current = current->next;
    current->next = new;
}


void append_letter_list(struct letter_list** letter_l, struct letter_list* new){
    if(!letter_l) errx(EXIT_FAILURE, "append_letter_list : letter_l doesn't exist");
    
    if(!(*letter_l)) {
        *letter_l = new;
        return;
    }

    struct letter_list* current = *letter_l;
    while(current->next) current = current->next;
    current->next = new;
}

/* ========================================
        basic processing
======================================== */

struct simple_surface* make_chunk(struct simple_surface* simple, int x1, int y1, int x2, int y2){
    /*
     * returns a struct simple_surface
     * containing the pixels of the chunk from (x1,y1) included to (x2,y2) included
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

    if (x1 < 0 || x2 >= simple->w || y1 < 0 || y2 >= simple->h)
        errx(EXIT_FAILURE, "make_chunk : Input error, invalid x or y");

    struct simple_surface* chunk = init_simple_surface(x2 - x1 + 1, y2 - y1 + 1);
    if (!chunk) 
        errx(EXIT_FAILURE, "init_simple_surface");

    for (int x = 0; x < chunk->w; x++){
        for (int y = 0; y < chunk->h; y++)
            chunk->gray[y*chunk->w + x] = simple->gray[(y+y1)*simple->w + (x+x1)];
    }

    return chunk;
}

struct simple_surface* rotate_picture(struct simple_surface* simple, double angle){
    /*
     * returns a struct simple_surface
     * which is the simple parameter but rotated. nothing was freed
     */


    double background = 0.0;
    int tot = simple->w * simple->h;
    for(int i = 0; i < tot ; i++) background += simple->gray[i];
    background = background / tot;

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
                pixel = background;
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
     * returns the Uint32 value of a pixel
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
     * directly modifies a pixel in a surface
     */

    if(!surface) errx(EXIT_FAILURE, "set_pixel : no surface");
    Uint8* p = (Uint8*)(surface->pixels) 
            + y*surface->pitch 
            + x*surface->format->BytesPerPixel;
    memcpy(p, &pixel, surface->format->BytesPerPixel);
}
