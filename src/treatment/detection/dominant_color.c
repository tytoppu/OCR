#include "dominant_color.h"
#include <err.h>
#include <stdlib.h>

DominantColor detect_writing_color(struct simple_surface* simple)
{
    int black_pixels = 0;
    int white_pixels = 0;
    Uint8 pixel;

    if (!simple)
        err(EXIT_FAILURE, "detect_writing_color : wrong input");

    for (int y = 0; y < simple->h; y++)
    {
        for (int x = 0; x < simple->w; x++)
        {
            pixel = simple->gray[y * simple->w + x];
            if (pixel != 0 && pixel != 255) err(EXIT_FAILURE, "detect_writing_color : simple-> not black and white");

            if (pixel == 0)
                black_pixels++;
            else
                white_pixels++;
        }
    }

    if (black_pixels > white_pixels)
        return WHITE;

    return BLACK;
}
