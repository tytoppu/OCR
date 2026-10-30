#include "filter.h"
#include "bitmap.h"
#include <err.h>
#include <stdlib.h>
#include <string.h>

/* 
    this file is for all the functions used for the color preprocessing
    of an image, pixel by pixel, to make it more readable for the ai
    and our preprocessing algorithms (rotation, cutting...)
*/



/* ========================================
            helpers
======================================== */

void contiguous (struct simple_surface* simple, int histogram[256], int* lower_pix, int* higher_pix){
    /*
        computes the lower_pix and higher_pix values of the pixels
        at CONTIGUITY % from the ends (changeable ?)
    */

    int threshold = simple->w * simple->h * CONTIGUITY;     // computes the contiguity

    int i = 0;
    int histo_sum = histogram[i];
    while (histo_sum <= threshold && i < 255){
        i ++;
        histo_sum += histogram[i];
    }
    *lower_pix = i;

    i = 255;
    histo_sum = histogram[i];
    while (histo_sum <= threshold && i < 255){
        i --;
        histo_sum += histogram[i];
    }
    *higher_pix = i;
}


void insert_array(Uint8 array[9], int* size, Uint8 elem){
    /*
        inserts an element in the array so that it stays
        sorted in ascending order
    */

    size_t i = 8;
    while (i > 0 && array[i-1] > elem){
        array[i] = array[i-1];
        i--;
    }
    array[i] = elem;
    (*size) ++;
}

/* ========================================
                grayscale
======================================== */


Uint8 single_grayscale(Uint8 r, Uint8 g, Uint8 b){
    /*
        returns the grayscale value of a pixel based on
        its RGB
    */

    return (Uint8)(0.299 * r + 0.587 * g + 0.114 * b + 0.5);
}



struct simple_surface* start_grayscale(SDL_Surface* surface){
    /*
     * turns an image given as a surface into a struct simple_surface
     * by putting all the pixels in shades of gray
     */

    int w = surface->w;
    int h = surface->h;
    struct simple_surface* grayscale = init_simple_surface(w, h);

    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint32 pixel;

    int x = 0;      // position on the width (starts on the left)
    int y = 0;      // position on the height (starts at the top)

    while(y < h){
        x = 0;
        while(x < w){
            pixel = get_pixel(surface, x, y);
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            grayscale->gray[y * w + x] = single_grayscale(r, g, b);

            x++;
        }
        y++;
    }
    return grayscale;
}


/* ========================================
            median filter
======================================== */

Uint8 single_median_filter(struct simple_surface* simple, int x, int y){
    /*
     * returns the median value in a 3x3 square around
     * the target pixel
     */

    int w = simple->w;
    int h = simple->h;
    Uint8* gray = simple->gray;

    int size = 0;                   // number of values stored
    Uint8 array[9];       // array sorted in ascending order with the grayscale values of the pixels
    for (int r = 0; r < 9; r++) {
        array[r] = 255;
    }

    insert_array(array, &size, gray[y * w + x]);    // inserts the gray value of the target pixel pointed by gray into the array (updating the size)

    if (x > 0) {
        insert_array(array, &size, gray[y * w + x - 1]);
        if (y > 0) insert_array(array, &size, gray[(y-1) * w + x-1]);
        if (y < h - 1) insert_array(array, &size, gray[(y+1) * w + x-1]);
    }
    if (x < w - 1) {
        insert_array(array, &size, gray[y * w + x+1]);
        if (y > 0)insert_array(array, &size, gray[(y-1) * w + x+1]);
        if (y < h - 1) insert_array(array, &size, gray[(y+1) * w + x+1]);
    }

    if (y > 0) insert_array(array, &size, gray[(y-1) * w + x]);
    if (y < h - 1) insert_array(array, &size, gray[(y+1) * w + x]);

    // returns the median
    if (size%2 == 0) return (array[size/2 - 1] + array[size/2]) / 2;
    else return array[size/2];
}


struct simple_surface* median_filter(struct simple_surface* simple, int histogram[256]){
    /*
        takes a surface, creates a new empty surface called new,
        fills it pixel by pixel with the median filter,
        frees the old surface and returns the new one
    */

    int w = simple->w;
    int h = simple->h;
    struct simple_surface* median = init_simple_surface(w, h);

    Uint8 gray_median;
    int y = 0;
    int x = 0;
    while(y < h){
        x = 0;
        while(x < w){
            gray_median = single_median_filter(simple, x, y);
            histogram[gray_median] ++;
            median->gray[y*w + x] = gray_median;

            x++;
        }
        y++;
    }
    return median;
}


/* ========================================
                contrast
======================================== */




Uint8 single_contrast(Uint8 gray, Uint8 min, Uint8 max){
    /*
        returns the grayscale value of a pixel,
        after "contrasting" based on the 5% min and max pixels
    */

    int answer = (gray - min) * 255 / (max - min);
    if (answer<0) return 0;
    else if (answer>255) return 255;
    return answer;
}

struct simple_surface* contrast(struct simple_surface* simple, int histogram[256]){
    /*
     * contrasts all the pixels of a simple_surface,
     * based on a histogram, with CONTIGUITY % contiguity
     * returns a new struct simple_surface
    */

    int w = simple->w;
    int h = simple->h;
    struct simple_surface* contrasted = init_simple_surface(w, h);

    int lower_pix = -1;
    int higher_pix = -1;
    contiguous (simple, histogram, &lower_pix, &higher_pix);

    if (higher_pix <= lower_pix) {
        memcpy(contrasted->gray, simple->gray, w * h);
        return contrasted;
    }

    //for(size_t i = 0; i<256; i++) histogram[i] = 0;
    Uint8 gray;
    int y = 0;
    int x = 0;
    while(y < h){
        x = 0;
        while(x < w){
            gray = single_contrast(simple->gray[y*w + x], lower_pix, higher_pix);
            //histogram[gray]++;
            contrasted->gray[y*w + x] = gray;

            x++;
        }
        y++;
    }
    return contrasted;
}



/* =======================================
            black_and_white
======================================== */

Uint8 single_black_and_white(Uint8 gray){
    /*
     * returns whether a pixel must be white or black
     */

    if (gray < 128) return 0;
    return 255;
}


struct simple_surface* black_and_white(struct simple_surface* simple, Uint8* anchor){
    /*
     * binarizes an image
     */

    int w = simple->w;
    int h = simple->h;
    struct simple_surface* bin = init_simple_surface(w, h);

    Uint8 black;

    int black_and_white[2] = {0};
    int y = 0;
    int x = 0;
    while(y < h){
        x = 0;
        while(x < w){
            black = single_black_and_white(simple->gray[y*w + x]);
            bin->gray[y*w + x] = black;
            black_and_white[black==255] ++;
            x++;
        }
        y++;
    }
    if(black_and_white[0] >= black_and_white[1]) *anchor = 255;
    else *anchor = 0;
    return bin;
}

/* =======================================
            rotate image
======================================== */

/*
int evaluate_rotation(struct simple_surface simple, double angle){
     * returns a struct simple_surface
     * which is the simple parameter but rotated. nothing was freed

    double theta = angle * M_PI / 180.0;
    double c = cos(theta);
    double s = sin(theta);


    int new_w = (int)round(fabs(simple->w * c) + fabs(simple->h * s)) + 1;
    int new_h = (int)round(fabs(simple->w * s) + fabs(simple->h * c)) + 1;

    struct simple_surface* rotate = init_simple_surface(new_w, new_h);
    if (!rotate) 
        errx(EXIT_FAILURE, "init_simple_surface failure in rotate_picture");

    double cx = simple->w/2.0;
    double cy = simple->h/2.0;
    double cnew_x = new_w/2.0;
    double cnew_y = new_h/2.0;

    Uint8 pixel;
    int y = 0;
    int x = 0;
    int x_s;
    int y_s;
    while(y < new_h){
        x = 0;
        while(x < new_w){

            x_s = (int)round(c * (x - cnew_x) + s * (y - cnew_y) + cx);
            y_s = (int)round(-s * (x - cnew_x) + c * (y - cnew_y) + cy);

            if (x_s >= 0 && x_s < simple->w && y_s >= 0 && y_s < simple->h)
                pixel = simple->gray[y_s * simple->w + x_s];
            else 
                pixel = background;
            rotate->gray[y * new_w + x] = pixel;

            x++;
        }
        y++;
    }
    return rotate;
}
*/

/* ========================================
            MAIN : normalizer
======================================== */


struct simple_surface* normalizer(SDL_Surface *surface, double angle, Uint8* anchor){
    /*
     * all the processing steps of an image,
     * after being loaded, a surface is processed
     * then we end by returning a struct simple_surface
     */

    save_picture(surface, "result_test/0.bmp");

    int histogram[256] = {0};

    struct simple_surface* grayscale = start_grayscale(surface);

    struct simple_surface* rotate = rotate_picture(grayscale, angle, 255);
    save_simple(rotate, "result_test/getrotated.bmp");
    destroy_simple_surface(grayscale);

    struct simple_surface* median = median_filter(rotate, histogram);
    destroy_simple_surface(rotate);

    struct simple_surface* contrasted = contrast(median, histogram);
    destroy_simple_surface(median);

    ;
    struct simple_surface* binary = black_and_white(contrasted, anchor);
    destroy_simple_surface(contrasted);

    if(!contrasted) err(EXIT_FAILURE, "normalizer FAILED");
    return binary;
}
