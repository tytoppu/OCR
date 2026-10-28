#include "image.h"


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



void test_normalizer(SDL_Surface* surface){
    struct simple_surface* simple = normalizer(surface);
    easy_save(simple, "result_test/supra_grayscale.bmp");
}


void test_make_chunck(struct simple_surface* init, int x1, int y1, int x2, int y2){
    struct simple_surface* simple = make_chunck(init, x1, y1, x2, y2);
    easy_save(simple, "result_test/chunk.bmp");
}


void test_rotate_picture(struct simple_surface* simple, double angle, Uint8 font){
    struct simple_surface* new = rotate_picture(simple, angle, font);
    easy_save(new, "result_test/getrotated.bmp");
}


/* ========================================
                MAIN
======================================== */


int main(int argc, char *argv[])
{
    if (argc != 2) errx(EXIT_FAILURE, "Wrong input");
    SDL_Surface* surface = load_standar_picture (argv[1]);
    if (!surface) errx(EXIT_FAILURE, "Wrong input");

    struct simple_surface* simple = start_grayscale(surface);
    if (!simple) errx(EXIT_FAILURE, "start_grayscale");

    test_normalizer(surface);

    test_make_chunck(simple, 10, 0, 99, 499);

    test_rotate_picture(simple, 0, 125);

    SDL_FreeSurface(surface);
    destroy_simple_surface(simple);
    printf("We are alive, all test past\n");
}
