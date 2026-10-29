#include "word_list_detection.h"

#include <stddef.h>
#include <stdlib.h>


/* The image contains one byte per pixel:
black = 0 white = 255. */




#define INK_THRESHOLD 128
#define MAX_BLANK_GAP 1

static int is_ink(const unsigned char *image,
                  int stride,
                  int x,
                  int y,
		  int colors)
{
    return image[(size_t)y * (size_t)stride + (size_t)x]
           ==colors;
}

static void set_rectangle(Rectangle *r,
                          int x_min,
                          int y_min,
                          int x_max,
                          int y_max)
{
    r->top_left.x = x_min;
    r->top_left.y = y_min;

    r->top_right.x = x_max;
    r->top_right.y = y_min;

    r->bottom_right.x = x_max;
    r->bottom_right.y = y_max;

    r->bottom_left.x = x_min;
    r->bottom_left.y = y_max;
}


/* Calculates the bounding box of the dark pixels in an area. */

static int find_bounds_in_region(const unsigned char *image,
                                 int width,
                                 int height,
                                 int stride,
                                 int x_start,
                                 int x_end,
                                 int *min_x,
                                 int *min_y,
                                 int *max_x,
                                 int *max_y,
                                 int *ink_count,
				 int colors)
{
    *min_x = width;
    *min_y = height;
    *max_x = -1;
    *max_y = -1;
    *ink_count = 0;

    if (x_start < 0)
        x_start = 0;

    if (x_end >= width)
        x_end = width - 1;

    if (x_start > x_end)
        return 0;

    for (int y = 0; y < height; ++y) {
        for (int x = x_start; x <= x_end; ++x) {
            if (is_ink(image, stride, x, y,colors)) {
                if (x < *min_x)
                    *min_x = x;

                if (x > *max_x)
                    *max_x = x;

                if (y < *min_y)
                    *min_y = y;

                if (y > *max_y)
                    *max_y = y;

                ++(*ink_count);
            }
        }
    }

    return *ink_count > 0;
}


/*
* Detects the overall boundaries of the word list.
* The list can be on the left or right side of the grid.
*/





int detect_word_list_bounds(const unsigned char *image,
                            int width,
                            int height,
                            int stride,
                            const GridCoordinates *grid,
                            WordListCoordinates *word_list,
			    int colors)
{
    int lminx, lminy, lmaxx, lmaxy, lcount;
    int rminx, rminy, rmaxx, rmaxy, rcount;
    int grid_left, grid_right;

    if (!image || !grid || !word_list ||
        width <= 0 || height <= 0 || stride < width)
        return 0;

    grid_left = grid->bounds.top_left.x;
    grid_right = grid->bounds.top_right.x;

    if (grid_left < 0 ||
        grid_right >= width ||
        grid_left >= grid_right)
        return 0;

    find_bounds_in_region(image,
                          width, height, stride,
                          0, grid_left - 1,
                          &lminx, &lminy,
                          &lmaxx, &lmaxy,
                          &lcount,colors);

    find_bounds_in_region(image,
                          width, height, stride,
                          grid_right + 1, width - 1,
                          &rminx, &rminy,
                          &rmaxx, &rmaxy,
                          &rcount,colors);


    /* Chooses the side containing the most dark pixels. */


    if (lcount == 0 && rcount == 0)
        return 0;

    if (lcount > 0 &&
        (rcount == 0 || lcount >= rcount)) {
	if (colors==0)
        	word_list->side = 255;
	else
		word_list->side=0;

        set_rectangle(&word_list->bounds,
                      lminx, lminy, lmaxx, lmaxy);

    } else {
	if (colors==0)
        	word_list->side = 255 ;
	else
		word_list->side=0;

        set_rectangle(&word_list->bounds,
                      rminx, rminy, rmaxx, rmaxy);
    }

    return 1;
}



/*
* Detects the boundaries of each word.
*
* Hypothesis:
* - one word per line;
* - a single column of words;
* - word lines are separated by whitespace.
*/



int detect_words_bounds(const unsigned char *image,
                        int width,
                        int height,
                        int stride,
                        const WordListCoordinates *word_list,
                        DetectedWord **words,
                        int *word_count,
			int colors)
{
    int x_min, x_max, y_min, y_max;
    int capacity = 0;
    int in_band = 0;
    int band_start = 0;
    int blank_rows = 0;

    DetectedWord *result = NULL;

    if (!words || !word_count)
        return 0;

    *words = NULL;
    *word_count = 0;

    if (!image || !word_list ||
        width <= 0 || height <= 0 || stride < width)
        return 0;

    x_min = word_list->bounds.top_left.x;
    x_max = word_list->bounds.top_right.x;
    y_min = word_list->bounds.top_left.y;
    y_max = word_list->bounds.bottom_left.y;

    if (x_min < 0 || x_max >= width ||
        y_min < 0 || y_max >= height ||
        x_min > x_max || y_min > y_max)
        return 0;




/*
* Loops through the pixel lines of the area.
* The extra margin helps to properly close
* the last line of text.
*/
    int inv;
    if (colors==0)
    	inv=250;
    else
	inv=0;


    for (int y = y_min;
         y <= y_max + inv + 1;
         ++y) {

        int row_has_ink = 0;

        if (y <= y_max) {
            for (int x = x_min; x <= x_max; ++x) {
                if (is_ink(image, stride, x, y,colors)) {
                    row_has_ink = 1;
                    break;
                }
            }
        }

        if (row_has_ink) {
            if (!in_band) {
                band_start = y;
                in_band = 1;
            }

            blank_rows = 0;

        } else if (in_band) {
            ++blank_rows;

            if (blank_rows > inv) {
                int band_end = y - blank_rows;
                int word_min_x = width;
                int word_max_x = -1;


		/*
* Searches for the horizontal limits
* of the word in the detected strip.
*/

                for (int yy = band_start;
                     yy <= band_end;
                     ++yy) {

                    for (int x = x_min; x <= x_max; ++x) {
                        if (is_ink(image, stride, x, yy,colors)) {
                            if (x < word_min_x)
                                word_min_x = x;

                            if (x > word_max_x)
                                word_max_x = x;
                        }
                    }
                }

		
		/* Adds the word to the dynamic array. */

                if (word_max_x >= word_min_x) {

                    if (*word_count == capacity) {
                        int new_capacity =
                            capacity == 0 ? 8 : capacity * 2;

                        DetectedWord *tmp = realloc(
                            result,
                            (size_t)new_capacity * sizeof(*result)
                        );

                        if (!tmp) {
                            free(result);
                            *word_count = 0;
                            return 0;
                        }

                        result = tmp;
                        capacity = new_capacity;
                    }

                    DetectedWord *word =
                        &result[*word_count];

                    set_rectangle(&word->bounds,
                                  word_min_x,
                                  band_start,
                                  word_max_x,
                                  band_end);

                    word->word = NULL;
                    word->letter_count = 0;
                    word->letters = NULL;

                    ++(*word_count);
                }

                in_band = 0;
                blank_rows = 0;
            }
        }
    }

    if (*word_count == 0) {
        free(result);
        return 0;
    }

    *words = result;

    return 1;
}


/* Free the memory allocated for the detected words. */

void free_detected_words(DetectedWord *words,
                         int word_count)
{
    if (!words)
        return;

    for (int i = 0; i < word_count; ++i) {
        free(words[i].word);
        free(words[i].letters);
    }

    free(words);
}
