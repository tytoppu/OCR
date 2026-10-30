
#ifndef WORD_LIST_DETECTION_H
#define WORD_LIST_DETECTION_H

#include "simple_surface.h"
#include "detection_data.h"
#include "bitmap.h"


void detect_words_bounds(struct simple_surface* list, struct word_list** word,Uint8 anchor);
void detect_word_letters(struct simple_surface* word, struct letter_list** letters, Uint8 anchor);
void detect_everything_in_list(struct simple_surface* list, struct word_list** word,Uint8 anchor);


#endif /* WORD_LIST_DETECTION_H */

