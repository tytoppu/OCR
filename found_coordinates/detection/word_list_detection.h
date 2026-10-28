
#ifndef WORD_LIST_DETECTION_H
#define WORD_LIST_DETECTION_H

#include "../structures/coordinates.h"
#include "../structures/detection_data.h"

/**
Detects the boundaries of the area containing the list of words.

word_list Coordinates of the overall list area.
*
returns 1 if the detection succeeds, 0 in case of failure.
*/


int detect_word_list_bounds(const unsigned char *image,
                            int width,
                            int height,
                            int stride,
                            const GridCoordinates *grid,
                            WordListCoordinates *word_list);



/*
* Detects the boundaries of each word in the list area.
1 if detection succeeds, 0 if it fails.
*/


int detect_words_bounds(const unsigned char *image,
                        int width,
                        int height,
                        int stride,
                        const WordListCoordinates *word_list,
                        DetectedWord **words,
                        int *word_count);



/*
Frees the memory of the array of detected words.
*/

void free_detected_words(DetectedWord *words, int word_count);

#endif /* WORD_LIST_DETECTION_H */

