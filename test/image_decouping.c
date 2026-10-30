#include "simple_surface.h"
#include "bitmap.h"
#include "cell_detection.h"
#include "image.h"
#include "filter.h"
#include "new_grid_list_detection.h"

#include  <stdlib.h>
#include  <err.h>

/* ========================================
                helper
======================================== */


void easy_save(struct simple_surface* simple, const char* path){
    save_simple(simple, path);
    destroy_simple_surface(simple);
    printf("Image saved as %s\n", path);
}


/* ========================================
                tests
======================================== */




void  test_cell_size_detector(struct simple_surface* simple, Uint8 ancre, int x1, int y1, int x2, int y2, char axe){
    struct simple_surface* new_simple = make_chunck(simple, x1, y1, x2, y2);
    int wide = cell_size_detector(new_simple, ancre, axe);
    struct simple_surface* result = NULL;
    if (axe == 'x') result = make_chunck(simple, x1, y1, x1+wide, y2);
    if (axe == 'y') result = make_chunck(simple, x1, y1, x2, y1+wide);
    easy_save(new_simple, "result_test/chucking.bmp");
    easy_save(result, "result_test/largeur.bmp");
}


void test_decouping(struct simple_surface* simple, Uint8 ancre){

    struct simple_surface* grid = NULL;
    struct simple_surface* list = NULL;

    detect_grid_and_list(simple, &grid, &list, ancre);
    struct cell_grid* easy = NULL;
    build_cells(grid, &easy, ancre);

    char* name = NULL;
    for (int y = 0; y<easy->h; y++){
        for (int x = 0; x<easy->w; x++){
            name = naming_point_bnp("result_test/test_image_decouping/letter", x, y);
            save_simple(easy->simple[x][y], name);
            free(name);
        }
    }
    destroy_cell_grid(easy);
}

void test_pretreatment(SDL_Surface* surface, int angle){
    pretreatment(surface, angle);
}



/* ========================================
                MAIN
======================================== */


int main(int argc, char *argv[])
{
    if (argc != 3) errx(EXIT_FAILURE, "Wrong input");
    SDL_Surface* surface = load_standar_picture (argv[1]);
    if (!surface) errx(EXIT_FAILURE, "Wrong input");
    //Uint8 ancre;
    //struct simple_surface* simple = normalizer(surface, &ancre);

    //test_image_decouping_CellCoordinates(surface);

    //test_cell_width_detector(simple, 0, 190, 25, 770, 605);
    //test_cell_height_detector(simple, 0, 190, 25, 770, 605);
    //test_cell_size_detector(simple, 0, 190, 25, 770, 605, 'y');
    //test_decouping(simple, ancre);
    
    //destroy_simple_surface(simple);

    test_pretreatment(surface, atof(argv[2]));
    SDL_FreeSurface(surface);

    printf("We are alive, all test past\n");
}
