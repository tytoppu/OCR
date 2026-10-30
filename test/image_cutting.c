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




void  test_cell_size_detector(struct simple_surface* simple, Uint8 anchor,
        int x1, int y1, int x2, int y2, char axis){
    struct simple_surface* new_simple = make_chunk(simple, x1, y1, x2, y2);
    int wide = cell_size_detector(new_simple, anchor, axis);
    struct simple_surface* result = NULL;
    if (axis == 'x') result = make_chunk(simple, x1, y1, x1+wide, y2);
    if (axis == 'y') result = make_chunk(simple, x1, y1, x2, y1+wide);
    easy_save(new_simple, "result_test/chunking.bmp");
    easy_save(result, "result_test/width.bmp");
}


void test_cutting(struct simple_surface* simple, Uint8 anchor){

    struct simple_surface* grid = NULL;
    struct simple_surface* list = NULL;

    detect_grid_and_list(simple, &grid, &list, anchor);
    struct cell_grid* easy = NULL;
    build_cells(grid, &easy, anchor);

    char* name = NULL;
    for (int y = 0; y<easy->h; y++){
        for (int x = 0; x<easy->w; x++){
            name = naming_point_bmp("result_test/test_image_cutting/letter",
                    x, y);
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
    SDL_Surface* surface = load_standard_picture (argv[1]);
    if (!surface) errx(EXIT_FAILURE, "Wrong input");
    //Uint8 anchor;
    //struct simple_surface* simple = normalizer(surface, &anchor);

    //test_image_cutting_CellCoordinates(surface);

    //test_cell_width_detector(simple, 0, 190, 25, 770, 605);
    //test_cell_height_detector(simple, 0, 190, 25, 770, 605);
    //test_cell_size_detector(simple, 0, 190, 25, 770, 605, 'y');
    //test_cutting(simple, anchor);
    
    //destroy_simple_surface(simple);

    test_pretreatment(surface, atof(argv[2]));
    SDL_FreeSurface(surface);

    printf("We are alive, all test past\n");
}
