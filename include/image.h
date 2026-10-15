#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stddef.h>

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


#endif