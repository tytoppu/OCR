
#ifndef LETTER_DETECTION_H
#define LETTER_DETECTION_H

#include "../structures/coordinates.h"
#include "../structures/detection_data.h"

//Détecte les lettres présentes dans la grille.
//Retourne 1 en cas de succès, 0 en cas d'erreur.
int detect_grid_letters(const DetectionData *data,
                        LetterCoordinates **letters,
                        int *letter_count);

//Détecte les lettres de chaque mot de la liste.
int detect_word_letters(const unsigned char *image,
                        int width,
                        int height,
                        int stride,
                        DetectedWord *words,
                        int word_count);

#endif /* LETTER_DETECTION_H */

