#include "../../../include/word_list_detection.h"
#include <err.h>
#include <stdlib.h>

/* The image contains one byte per pixel:
black = 0 white = 255. */




#define INK_THRESHOLD 128
#define MAX_BLANK_GAP 1

int is_ink(struct simple_surface* list,
                  int x,
                  int y,
		  int colors)
{
    return list->gray[(size_t)y * (size_t)list->w + (size_t)x]
           ==colors;
}


/*
* Detects the boundaries of each word.
*
* Hypothesis:
* - one word per line;
* - a single column of words;
* - word lines are separated by whitespace.
*/



void detect_words_bounds(struct simple_surface* list,
struct simple_surface**  word,int *word_count,
			int colors)
{
    int x_min, x_max, y_min, y_max;
    int capacity = 0;
    int in_band = 0;
    int band_start = 0;
    int blank_rows = 0;

    DetectedWord *result = NULL;

    if (!word_count)
        errx(EXIT_FAILURE, "detect_words_bounds : wrong input");

    *word_count = 0;

    if (!list || !word)
        errx(EXIT_FAILURE, "detect_words_bounds : wrong input");

    x_min = 0;
    x_max = list->w-1;
    y_min = 0;
    y_max = list->h-1;

    if (x_min < 0 || x_max >= list->w ||
        y_min < 0 || y_max >= list->h ||
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
                if (is_ink(list, x, y,colors)) {
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
                int word_min_x = list->w;
                int word_max_x = -1;


		/*
* Searches for the horizontal limits
* of the word in the detected strip.
*/

                for (int yy = band_start;
                     yy <= band_end;
                     ++yy) {

                    for (int x = x_min; x <= x_max; ++x) {
                        if (is_ink(list, x, yy,colors)) {
                            if (x < word_min_x)
                                word_min_x = x;

                            if (x > word_max_x)
                                word_max_x = x;
                        }
                    }
                }

		
		/* Adds the word to the dynamic array. */

                if (word_max_x >= word_min_x) {
			word[*word_count]->w=word_max_x - word_min_x + 1;
			word[*word_count]->h=band_end - band_start + 1;
			make_chunck(word[*word_count],word_max_x, band_start,word_min_x, band_end);
		}
	     }	    
	}
    }	    
}


/* Free the memory allocated for the detected words. */
/*
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
*/
