#ifndef DETECTION_DATA_H
#define DETECTION_DATA_H

#include "coordinates.h"



// coordinate of a square


typedef struct
{
    Rectangle bounds;
    int row;
    int column;
} CellCoordinates;


//coordinates of a letter

typedef struct
{
    Rectangle bounds;
} LetterCoordinates;



//coordinate of a word and its letters

typedef struct
{
    Rectangle bounds;
    char *word;
    int letter_count;
    LetterCoordinates *letters;
} DetectedWord;


//indicates the dominant color

typedef enum
{
	BLACK,
	WHITE
}DominantColor;



//gathers all results

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
