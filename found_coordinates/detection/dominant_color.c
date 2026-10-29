#include "dominant_color.h"

#include <stddef.h>


DominantColor detect_dominant_color(
    const unsigned char *image,
    int width,
    int height,
    int stride
)
{
    int black_pixels = 0;
    int white_pixels = 0;

    if (image == NULL || width <= 0 || height <= 0 || stride < width)
        return BLACK;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            unsigned char pixel = image[y * stride + x];

            if (pixel < 128)
                black_pixels++;
            else
                white_pixels++;
        }
    }

    if (black_pixels > white_pixels)
        return WHITE;

    return BLACK;
}
