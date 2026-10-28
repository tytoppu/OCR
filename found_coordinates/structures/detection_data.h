#ifndef DETECTION_DATA_H
#define DETECTION_DATA_H

#include "coordinates.h"


// coordonné d'une case

typedef struct
{
    Rectangle bounds;
    int row;
    int column;
} CellCoordinates;

//coordonné d'une lettre

typedef struct
{
    Rectangle bounds;
} LetterCoordinates;

//coordonné d'un mot et des ses lettre

typedef struct
{
    Rectangle bounds;
    char *word;
    int letter_count;
    LetterCoordinates *letters;
} DetectedWord;

//indique la couleur dominante
typedef enum
{
	BLACK,
	WHITE
}DominantColor;


//regroupe tout résultat
typedef struct
{
    GridCoordinates grid;
    WordListCoordinates word_list;

    int cell_count;
    CellCoordinates **cells;

    int grid_letter_count;
    LetterCoordinates *grid_letters;

    int word_count;
    DetectedWord *words;
} DetectionData;




#endif /* DETECTION_DATA_H */
