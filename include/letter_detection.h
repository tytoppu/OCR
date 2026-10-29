
#ifndef LETTER_DETECTION_H
#define LETTER_DETECTION_H

#include "coordinates.h"
#include "detection_data.h"




//Detects the letters present on the grid.
//Returns 1 on success, 0 on error.

int detect_grid_letters(const DetectionData *data,
                        LetterCoordinates **letters,
                        int *letter_count);



//Detects the letters of each word in the list.
int detect_word_letters(const unsigned char *image,
                        int width,
                        int height,
                        int stride,
                        DetectedWord *words,
                        int word_count,
			int color);

#endif /* LETTER_DETECTION_H */

