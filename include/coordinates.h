#ifndef COORDINATES_H
#define COORDINATES_H

//coordinate in pixels

typedef struct
{
    int x;
    int y;
} Point;


//the coordinates of an area


typedef struct
{
    Point top_left;
    Point top_right;
    Point bottom_right;
    Point bottom_left;
} Rectangle;


//grid info


//à sup
/*
typedef struct
{
    Rectangle bounds;
    int rows;
    int columns;
    int cell_width;
    int cell_height;
} GridCoordinates;
*/

//say if list of words on the right or on the left

//plus besoin
/*
typedef enum
{
    WORD_LIST_LEFT,
    WORD_LIST_RIGHT
} WordListSide;
*/
//coordinates of the word list and location

/*
typedef struct
{
    Rectangle bounds; //si on garde à remplir
    WordListSide side;  // à enlever
} WordListCoordinates;
*/
#endif /* COORDINATES_H */

