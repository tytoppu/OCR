#ifndef DOMINANT_COLOR_H
#define DOMINANT_COLOR_H


#include "../structures/detection_data.h"




DominantColor detect_dominant_color(
const unsigned char *image,
int width,
int height,
int stride
);





#endif /* DOMINANT_COLOR_H */
