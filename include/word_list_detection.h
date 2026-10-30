
#ifndef WORD_LIST_DETECTION_H
#define WORD_LIST_DETECTION_H

#include "simple_surface.h"
#include "detection_data.h"
#include "bitmap.h"


void detect_words_bounds(struct simple_surface* list, struct word_liste** word,Uint8 ancre);
void detect_word_letters(struct simple_surface* word, struct letre_list** letre, Uint8 ancre);
void detect_everything_in_list(struct simple_surface* list, struct word_liste** word,Uint8 ancre);


#endif /* WORD_LIST_DETECTION_H */

