
#ifndef LETTER_DETECTION_H
#define LETTER_DETECTION_H

#include "simple_surface.h"
#include "detection_data.h"


//Detects the letters present on the grid.
//Returns 1 on success, 0 on error.

void detect_grid_letters(const DetectionData *data,
                        LetterCoordinates **letters,
                        int *letter_count);



//Detects the letters of each word in the list.
void detect_word_letters(struct simple_surface* simple,
                        DetectedWord *words,
                        int word_count,
			int color);

#endif /* LETTER_DETECTION_H */

