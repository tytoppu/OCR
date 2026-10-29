#include "word_list_detection.h"
#include <err.h>
#include <stdlib.h>

/* The image contains one byte per pixel:
black = 0 white = 255. */




#define INK_THRESHOLD 128
#define MAX_BLANK_GAP 1

int is_ink(struct simple_surface* simple,
                  int x,
                  int y,
		  int colors)
{
    return simple->gray[(size_t)y * (size_t)simple->w + (size_t)x]
           ==colors;
}

void set_rectangle(Rectangle *r,
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

int find_bounds_in_region(struct simple_surface* simple,
                                 int x_start,
                                 int x_end,
                                 int *min_x,
                                 int *min_y,
                                 int *max_x,
                                 int *max_y,
                                 int *ink_count,
				 int colors)
{
    *min_x = simple->w;
    *min_y = simple->h;
    *max_x = -1;
    *max_y = -1;
    *ink_count = 0;

    if (x_start < 0)
        x_start = 0;

    if (x_end >= simple->w)
        x_end = simple->w - 1;

    if (x_start > x_end)
        errx(EXIT_FAILURE, "find_bounds_in_region : wrong input");

    for (int y = 0; y < simple->h; ++y) {
        for (int x = x_start; x <= x_end; ++x) {
            if (is_ink(simple, x, y,colors)) {
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





void detect_word_list_bounds(struct simple_surface* simple,
                            const GridCoordinates *grid,
                            WordListCoordinates *word_list,
			    int colors)
{
    int lminx, lminy, lmaxx, lmaxy, lcount;
    int rminx, rminy, rmaxx, rmaxy, rcount;
    int grid_left, grid_right;

    if (!simple || !word_list)
        errx(EXIT_FAILURE, "detect_word_list_bounds : wrong input");

    grid_left = grid->bounds.top_left.x;
    grid_right = grid->bounds.top_right.x;

    if (grid_left < 0 ||
        grid_right >= simple->w ||
        grid_left >= grid_right)
        errx(EXIT_FAILURE, "detect_word_list_bounds : wrong input in grid");

    find_bounds_in_region(simple,
                          0, grid_left - 1,
                          &lminx, &lminy,
                          &lmaxx, &lmaxy,
                          &lcount,colors);

    find_bounds_in_region(simple,
                          grid_right + 1, simple->w - 1,
                          &rminx, &rminy,
                          &rmaxx, &rmaxy,
                          &rcount,colors);


    /* Chooses the side containing the most dark pixels. */


    if (lcount == 0 && rcount == 0)
        errx(EXIT_FAILURE, "detect_word_list_bounds : wrong result in counts");

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

}



/*
* Detects the boundaries of each word.
*
* Hypothesis:
* - one word per line;
* - a single column of words;
* - word lines are separated by whitespace.
*/



void detect_words_bounds(struct simple_surface* simple,
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
        errx(EXIT_FAILURE, "detect_words_bounds : wrong input");

    *words = NULL;
    *word_count = 0;

    if (!simple || !word_list)
        errx(EXIT_FAILURE, "detect_words_bounds : wrong input");

    x_min = word_list->bounds.top_left.x;
    x_max = word_list->bounds.top_right.x;
    y_min = word_list->bounds.top_left.y;
    y_max = word_list->bounds.bottom_left.y;

    if (x_min < 0 || x_max >= simple->w ||
        y_min < 0 || y_max >= simple->h ||
        x_min > x_max || y_min > y_max)
        errx(EXIT_FAILURE, "detect_words_bounds : wrong input in word_list->bounds");




/*
* Loops through the pixel lines of the area.
* The extra margin helps to properly close
* the last line of text.
*/
    int inv;
    if (colors==0)
    	inv=255;
    else
	inv=0;


    for (int y = y_min;
         y <= y_max + inv + 1;
         ++y) {

        int row_has_ink = 0;

        if (y <= y_max) {
            for (int x = x_min; x <= x_max; ++x) {
                if (is_ink(simple, x, y,colors)) {
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
                int word_min_x = simple->w;
                int word_max_x = -1;


		/*
* Searches for the horizontal limits
* of the word in the detected strip.
*/

                for (int yy = band_start;
                     yy <= band_end;
                     ++yy) {

                    for (int x = x_min; x <= x_max; ++x) {
                        if (is_ink(simple, x, yy,colors)) {
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
                            errx(EXIT_FAILURE, "detect_words_bounds : realloc failed");
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
        errx(EXIT_FAILURE, "detect_words_bounds : *word_count == 0");
    }

    *words = result;
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
