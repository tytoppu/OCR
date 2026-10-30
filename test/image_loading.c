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


void test_normalizer_steps(SDL_Surface *surface){

    save_picture(surface, "result_test/0.bmp");

    int histogram[256] = {0};

    struct simple_surface* grayscale = start_grayscale(surface);
    save_simple(grayscale, "result_test/1.bmp");

    struct simple_surface* median = median_filter(grayscale, histogram);
    destroy_simple_surface(grayscale);
    save_simple(median, "result_test/2.bmp");

    struct simple_surface* contrasted = contrast(median, histogram);
    destroy_simple_surface(median);
    save_simple(contrasted, "result_test/3.bmp");


    struct simple_surface* binary = black_and_white(contrasted);
    destroy_simple_surface(contrasted);
    save_simple(binary, "result_test/4.bmp");

    destroy_simple_surface(binary);
}

void test_make_chunk(struct simple_surface* init, int x1, int y1, int x2, int y2){
    struct simple_surface* simple = make_chunk(init, x1, y1, x2, y2);
    easy_save(simple, "result_test/chunk.bmp");
}


void test_rotate_picture(struct simple_surface* simple, double angle, Uint8 background){
    struct simple_surface* new = rotate_picture(simple, angle, background);
    easy_save(new, "result_test/getrotated.bmp");
}


/* ========================================
                MAIN
======================================== */


int main(int argc, char *argv[])
{
    if (argc != 2) errx(EXIT_FAILURE, "Wrong input");
    SDL_Surface* surface = load_standard_picture (argv[1]);
    if (!surface) errx(EXIT_FAILURE, "Wrong input");

    //struct simple_surface* simple = start_grayscale(surface);
    //if (!simple) errx(EXIT_FAILURE, "start_grayscale");

    //test_normalizer(surface);

    //test_make_chunk(simple, 10, 0, 99, 499);

    //test_rotate_picture(simple, 0, 125);

    test_normalizer_steps(surface);

    SDL_FreeSurface(surface);
    //destroy_simple_surface(simple);
    printf("We are alive, all test past\n");
}
