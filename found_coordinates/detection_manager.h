
#ifndef DETECTION_MANAGER_H
#define DETECTION_MANAGER_H

#include "structures/detection_data.h"

//Lance toutes les étapes de détection
int run_detection(const unsigned char *image,
                  int width,
                  int height,
                  int stride,
                  int rows,
                  int columns,
                  DetectionData *data);

/* Libère la mémoire allouée dans DetectionData. */
void free_detection_data(DetectionData *data);

#endif /* DETECTION_MANAGER_H */

