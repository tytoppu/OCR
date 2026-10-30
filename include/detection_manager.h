
#ifndef DETECTION_MANAGER_H
#define DETECTION_MANAGER_H

#include "simple_surface.h"
#include "detection_data.h"

//Launch all detection steps
void run_detection(struct simple_surface* simple, struct cell_grid** cell, struct word_liste** word, Uint8 ancre);



#endif /* DETECTION_MANAGER_H */

