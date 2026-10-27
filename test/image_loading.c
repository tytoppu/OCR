#include "image.h"


void test_normalizer(SDL_Surface* surface){
    SDL_Surface* new_surface = normalizer(surface);
    save_image(new_surface, "result_test/supra_grayscale.bmp");
    printf("Image saved as supra_grayscale.bmp\n");
}


void test_chunck(SDL_Surface* surface){
    SDL_Surface* new = make_chunck(surface, 100, 100, 400, 400);
    save_image(new, "result_test/chunk.bmp");
    printf("Image saved as chunck.bmp\n");
}


void test_rotate(SDL_Surface* surface){
    SDL_Surface* new = rotate_picture(surface, 45, 126);
    //int histogram[256];
    //SDL_Surface* newnew = median_filter(new, histogram);

    save_image(new, "result_test/getrotated.bmp");
    printf("Image saved as getrotated.bmp\n");
}


int main(int argc, char *argv[])
{
    if (argc != 2) errx(EXIT_FAILURE, "Wrong input");
    SDL_Surface* surface = load_image(argv[1]);

    //test_normalizer(surface):

    //test_chunck(surface);

    test_rotate(surface);
}
