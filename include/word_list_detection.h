
#ifndef WORD_LIST_DETECTION_H
#define WORD_LIST_DETECTION_H

#include "simple_surface.h"
#include "detection_data.h"
#include "bitmap.h"






/*
* Detects the boundaries of each word in the list area.
1 if detection succeeds, 0 if it fails.
*/


void detect_words_bounds(struct simple_surface* list,struct simple_surface**  word,int *word_count,int colors);



#endif /* WORD_LIST_DETECTION_H */

