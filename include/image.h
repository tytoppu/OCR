#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stddef.h>

/*
 * Loads an uncompressed BMP (24 or 32 bits), converts it to grayscale,
 * resizes it to width x height, then normalizes each pixel
 * to [0.0, 1.0].
 *
 * `destination` must hold at least width * height doubles.
 *
 * Returns 1 on success, 0 on error.
 */
int load_bmp_image_double(const char *path,
                          double *destination,
                          size_t width,
                          size_t height);


#endif