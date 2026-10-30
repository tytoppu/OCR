#ifndef IMAGE_H
#define IMAGE_H

#include <stddef.h>
#include <SDL2/SDL.h>
#include "simple_surface.h"

/*
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

#include "bitmap.h"
#include "filter.h"
#include "cell_detection.h"
#include "coordinates.h"
#include "detection_data.h"
#include "detection_manager.h"
#include "dominant_color.h"
#include "grid_detection.h"
#include "letter_detection.h"
#include "word_list_detection.h"
*/


char* naming_point_bmp(const char *prefix, int x, int y);
void cell_grid_to_bmp(struct cell_grid* cell);
void word_list_to_bmp(struct word_list* word);
//void pretreatment(SDL_Surface* surface, struct cell_grid** cell, struct word_list** word);
void pretreatment(SDL_Surface* surface, double angle);


#endif  /* IMAGE_H */