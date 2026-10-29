#include "image.h"
#include "cell_detection.h"
#include "coordinates.h"
#include "detection_data.h"
#include "detection_manager.h"
#include "dominant_color.h"
#include "grid_detection.h"
#include "letter_detection.h"
#include "word_list_detection.h"

/*
 * normalise l'image
 * on la decoupe
    * la grille -> les letre de la grille
    * les mots -> les letre des mots
 * on renvoi les simple_surface associer -> conversion en surface -> conversion en image 
*/

char* naming_point_bnp(const char* prefix, int x, int y) {
    
    int len = snprintf(NULL, 0, "%s_%d_%d", prefix, x, y);
    if (len < 0) return NULL;
    len++;
    char* new_char = malloc(len);  // +1 pour '\0'
    if (!new_char) return NULL;

    snprintf(new_char, len, "%s_%d_%d.bnp", prefix, x, y);
    return new_char;
}

void pretreatment(SDL_Surface* surface){
    struct simple_surface* normalized = normalizer(surface);

    SDL_Surface* normalized_surface = simple_surface_to_surface(normalized);

    DetectionData data = {0};
    if (run_detection(normalized_surface->pixels, normalized_surface->w, normalized_surface->h, normalized_surface->pitch, &data) != 1){
        errx(EXIT_FAILURE, "pretreatment");
    }

    int x = 0;
    int y = 0;
    int rows = data.grid.rows;
    int columns = data.grid.columns;
    Rectangle bounds;
    struct simple_surface* chunck = NULL;
    char* path = NULL;

    while(y<rows){
        x = 0;
        while(x<columns){
            bounds = data.cells[y][x].bounds;
            chunck = make_chunck(normalized, bounds.top_left.x, bounds.top_left.y, 
                bounds.bottom_right.x, bounds.bottom_right.y);
            path = naming_point_bnp("result_test/image_decouping/CellCoordinates/", x, y);
            save_simple(chunck, path);
            free(path);
            x++;
        }
        y++; 
    } 

}



/*

int detect_grid_dimensions(
    const unsigned char *image,
    int width,
    int height,
    int stride,
    GridCoordinates *grid,
    int colors
);







struct simple_surface* make_chunck(struct simple_surface* simple, int x1, int y1, int x2, int y2)
void destroy_simple_surface(struct simple_surface* simple)
void save_simple(struct simple_surface* simple, const char *path)

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

typedef struct
{
    Rectangle bounds;
    int rows;
    int columns;
    int cell_width;
    int cell_height;
} GridCoordinates


typedef struct
{
    Rectangle bounds;
    WordListSide side;
} WordListCoordinates;



typedef struct
{
    int x;
    int y;
} Point;


typedef struct
{
    Point top_left;
    Point top_right;
    Point bottom_right;
    Point bottom_left;
} Rectangle;






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

*/