
#ifndef DETECTION_MANAGER_H
#define DETECTION_MANAGER_H

#include "detection_data.h"


//Launch all detection steps
int run_detection(const unsigned char *image,
                  int width,
                  int height,
                  int stride,
       
                  DetectionData *data);

/* Frees the memory allocated in DetectionData. */
void free_detection_data(DetectionData *data);

#endif /* DETECTION_MANAGER_H */

