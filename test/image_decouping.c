#include "image.h"


/* ========================================
                helper
======================================== */





/* ========================================
                tests
======================================== */



void test_image_decouping_CellCoordinates(SDL_Surface* surface){
    pretreatment(surface);
}


/* ========================================
                MAIN
======================================== */


int main(int argc, char *argv[])
{
    if (argc != 2) errx(EXIT_FAILURE, "Wrong input");
    SDL_Surface* surface = load_standar_picture (argv[1]);
    if (!surface) errx(EXIT_FAILURE, "Wrong input");

    test_image_decouping_CellCoordinates(surface);

    SDL_FreeSurface(surface);

    printf("We are alive, all test past\n");
}
