//#include "simple_surface.h"
#include "bitmap.h"
//#include "cell_detection.h"
#include "image.h"
//#include "filter.h"
//#include "new_grid_list_detection.h"

#include <stdlib.h>
#include  <err.h>

int main(int argc, char *argv[]){
    if (argc != 3) errx(EXIT_FAILURE, "Wrong input");
    SDL_Surface* surface = load_standard_picture (argv[1]);
    if (!surface) errx(EXIT_FAILURE, "Wrong input");

    pretreatment(surface, atof(argv[2]));

}