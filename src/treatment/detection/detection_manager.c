#include "detection_manager.h"
#include "cell_detection.h"
#include "new_grid_list_detection.h"
#include "word_list_detection.h"
#include <err.h>
#include <stdlib.h>
#include <string.h>






void run_detection(struct simple_surface* simple, struct cell_grid** cell, struct word_list** word, Uint8 anchor){

    if (!simple) err(EXIT_FAILURE, "run_detection : wrong input");

    struct simple_surface* grid = NULL;
    struct simple_surface* list = NULL;
    detect_grid_and_list(simple, &grid, &list, anchor);

    build_cells(grid, cell, anchor);

    detect_everything_in_list(list, word, anchor);

    destroy_simple_surface(grid);
    destroy_simple_surface(list);

}

