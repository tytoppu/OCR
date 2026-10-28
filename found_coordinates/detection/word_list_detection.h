
#ifndef WORD_LIST_DETECTION_H
#define WORD_LIST_DETECTION_H

#include "../structures/coordinates.h"
#include "../structures/detection_data.h"

/**
  Detecte les limites de la zone contenant la liste de mots.
 
 word_list Coordonnees de la zone globale de la liste.
 *
 return 1 si la detection reussit, 0 en cas d'echec.
 */
int detect_word_list_bounds(const unsigned char *image,
                            int width,
                            int height,
                            int stride,
                            const GridCoordinates *grid,
                            WordListCoordinates *word_list);

/*
 * Detecte les limites de chaque mot dans la zone de la liste.
  1 si la detection reussit, 0 en cas d'echec.
 */
int detect_words_bounds(const unsigned char *image,
                        int width,
                        int height,
                        int stride,
                        const WordListCoordinates *word_list,
                        DetectedWord **words,
                        int *word_count);

/*
  Libere la memoire du tableau de mots detectes.
 */
void free_detected_words(DetectedWord *words, int word_count);

#endif /* WORD_LIST_DETECTION_H */

