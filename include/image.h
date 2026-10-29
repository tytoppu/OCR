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



void pretreatment(SDL_Surface *surface);
char* naming_point_bnp(const char *prefix, int x, int y);

/*
 * Charge un BMP non compresse (24 ou 32 bits), le convertit en niveaux
 * de gris, le redimensionne en largeur x hauteur, puis normalise chaque
 * pixel dans [0.0, 1.0].
 *
 * `destination` doit contenir au moins largeur * hauteur doubles.
 *
 * Retourne 1 en cas de succes, 0 en cas d'erreur.
 */
int charger_image_bmp_double(const char *chemin, double *destination,
                             size_t largeur, size_t hauteur);

#endif  /* IMAGE_H */