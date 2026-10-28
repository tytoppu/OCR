#ifndef COORDINATES_H
#define COORDINATES_H




//coordonné en pixel


typedef struct
{
    int x;
    int y;
} Point;


//les coordonnés d'une zones

typedef struct
{
    Point top_left;
    Point top_right;
    Point bottom_right;
    Point bottom_left;
} Rectangle;


//info de la grille

typedef struct
{
    Rectangle bounds;
    int rows;
    int columns;
    int cell_width;
    int cell_height;
} GridCoordinates;

//dis si liste de mots à droite ou à gauche

typedef enum
{
    WORD_LIST_LEFT,
    WORD_LIST_RIGHT
} WordListSide;



//coordinnées de la liste de mots et emplacement

typedef struct
{
    Rectangle bounds;
    WordListSide side;
} WordListCoordinates;

#endif /* COORDINATES_H */

