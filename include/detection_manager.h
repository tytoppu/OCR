
#ifndef DETECTION_MANAGER_H
#define DETECTION_MANAGER_H

#include "simple_surface.h"
#include "detection_data.h"

//Launch all detection steps
int run_detection(struct simple_surface* simple, DetectionData *data);

/* Frees the memory allocated in DetectionData. */
void free_detection_data(DetectionData *data);

#endif /* DETECTION_MANAGER_H */

