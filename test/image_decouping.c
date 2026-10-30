#include "simple_surface.h"
#include "bitmap.h"
#include "cell_detection.h"
#include "image.h"
#include "filter.h"

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


/*
void test_image_decouping_CellCoordinates(SDL_Surface* surface){
    pretreatment(surface);
}
*/
/*
void  test_cell_width_detector(struct simple_surface* simple, Uint8 ancre, int x1, int y1, int x2, int y2){
    printf("%i\n", ancre);
    struct simple_surface* new_simple = make_chunck(simple, x1, y1, x2, y2);
    printf("oui ?\n");
    int wide = cell_width_detector(new_simple, ancre);
    printf("x1 + wide %i, x1 %i\n",x1+wide, x1);
    struct simple_surface* result = make_chunck(simple, x1+4*wide, y1, x1+5*wide, y2);
    printf("ca marche ? \n");
    easy_save(new_simple, "result_test/chucking.bmp");
    easy_save(result, "result_test/largeur.bmp");
}

void  test_cell_height_detector(struct simple_surface* simple, Uint8 ancre, int x1, int y1, int x2, int y2){
    printf("%i\n", ancre);
    struct simple_surface* new_simple = make_chunck(simple, x1, y1, x2, y2);
    printf("oui ?\n");
    int wide = cell_height_detector(new_simple, ancre);
    printf("y1 + wide %i, y1 %i\n",y1+wide, y1);
    struct simple_surface* result = make_chunck(simple, x1, y1, x2, y1+wide);
    printf("ca marche ? \n");
    easy_save(new_simple, "result_test/chucking.bmp");
    easy_save(result, "result_test/largeur.bmp");
}
    */

void  test_cell_size_detector(struct simple_surface* simple, Uint8 ancre, int x1, int y1, int x2, int y2, char axe){
    struct simple_surface* new_simple = make_chunck(simple, x1, y1, x2, y2);
    int wide = cell_size_detector(new_simple, ancre, axe);
    struct simple_surface* result = NULL;
    if (axe == 'x') result = make_chunck(simple, x1, y1, x1+wide, y2);
    if (axe == 'y') result = make_chunck(simple, x1, y1, x2, y1+wide);
    easy_save(new_simple, "result_test/chucking.bmp");
    easy_save(result, "result_test/largeur.bmp");
}



/* ========================================
                MAIN
======================================== */


int main(int argc, char *argv[])
{
    if (argc != 2) errx(EXIT_FAILURE, "Wrong input");
    SDL_Surface* surface = load_standar_picture (argv[1]);
    if (!surface) errx(EXIT_FAILURE, "Wrong input");
    struct simple_surface* simple = normalizer(surface);

    //test_image_decouping_CellCoordinates(surface);

    printf("%i, %i\n\n\n\n", simple->w, simple->h);
    //test_cell_width_detector(simple, 0, 190, 25, 770, 605);
    //test_cell_height_detector(simple, 0, 190, 25, 770, 605);
    test_cell_size_detector(simple, 0, 190, 25, 770, 605, 'y');
    
    
    destroy_simple_surface(simple);

    SDL_FreeSurface(surface);

    printf("We are alive, all test past\n");
}
