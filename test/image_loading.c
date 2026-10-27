#include "image.h"

int main(int argc, char *argv[])
{
    if (argc != 2) errx(EXIT_FAILURE, "Wrong input");

    SDL_Surface* surface = load_image(argv[1]);
    SDL_Surface* new_surface = normalizer(surface);
    save_image(new_surface, "supra_grayscale.bmp");
    printf("Image saved as grayscale.bmp\n");
}
