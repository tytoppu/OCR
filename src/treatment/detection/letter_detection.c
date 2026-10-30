#include "letter_detection.h"
#include <err.h>
#include <stdlib.h>
/*
* Copies the coordinates of the grid squares
* to use them as coordinates for the letters.
*/


void detect_grid_letters(const DetectionData *data,
                        LetterCoordinates **letters,
                        int *letter_count)
{
    int row;
    int column;
    int index;

    if (data == NULL || letters == NULL || letter_count == NULL)
        errx(EXIT_FAILURE, "detect_grid_letters : wrong input");

    *letters = NULL;
    *letter_count = 0;

    if (data->cells == NULL ||
        data->grid.rows <= 0 ||
        data->grid.columns <= 0)
        errx(EXIT_FAILURE, "detect_grid_letters : wrong input in data");

    *letter_count = data->grid.rows * data->grid.columns;

    *letters = malloc(*letter_count * sizeof(LetterCoordinates));

    if (*letters == NULL)
    {
        *letter_count = 0;
        errx(EXIT_FAILURE, "detect_grid_letters : wrong input in letters");
    }

    index = 0;

    for (row = 0; row < data->grid.rows; row++)
    {
        for (column = 0; column < data->grid.columns; column++)
        {
            (*letters)[index].bounds =
                data->cells[row][column].bounds;

            index++;
        }
    }

}


//Identify the letters of a word by grouping the black pixels

void detect_word_letters(struct word_liste* word_l, 
			struct letre_liste* letre_l,
                        int word_count,
			int colors)
{
    int w;

    if (!simple || words == NULL ||
        word_count < 0)
        errx(EXIT_FAILURE, "detect_word_letters : wrong input");

    for (w = 0; w < word_count; w++)
    {
        int x_min = 0;
        int y_min = words[w].bounds.top_left.y;
        int x_max = words[w].bounds.top_right.x;
        int y_max = words[w].bounds.bottom_left.y;
        int x;
        int letter_count = 0;
        int in_letter = 0;
        int start_x = 0;

        if (x_min < 0) x_min = 0;
        if (y_min < 0) y_min = 0;
        if (x_max > simple->w) x_max = simple->w;
        if (y_max > simple->h) y_max = simple->h;

        if (x_min >= x_max || y_min >= y_max)
            errx(EXIT_FAILURE, "detect_word_letters : wrong result");


	// First pass: count the groups of black pixels.
         
        for (x = x_min; x < x_max; x++)
        {
            int has_black = 0;
            int y;

            for (y = y_min; y < y_max; y++)
            {
                if (simple->gray[y * simple->w + x] ==colors)
                {
                    has_black = 1;
                    break;
                }
            }

            if (has_black && !in_letter)
            {
                letter_count++;
                in_letter = 1;
            }
            else if (!has_black)
            {
                in_letter = 0;
            }
        }

        if (letter_count == 0)
            errx(EXIT_FAILURE, "detect_word_letters : wrong result letter_count");

        words[w].letters = malloc(
            letter_count * sizeof(LetterCoordinates));

        if (words[w].letters == NULL)
            errx(EXIT_FAILURE, "detect_word_letters : wrong result at words[w].letters");

        words[w].letter_count = letter_count;

     	//records the horizontal limits

        in_letter = 0;
        letter_count = 0;

        for (x = x_min; x <= x_max; x++)
        {
            int has_black = 0;
            int y;

            if (x < x_max)
            {
                for (y = y_min; y < y_max; y++)
                {
                    if (simple->gray[y * simple->w + x] ==colors)
                    {
                        has_black = 1;
                        break;
                    }
                }
            }

            if (has_black && !in_letter)
            {
                start_x = x;
                in_letter = 1;
            }
            else if (!has_black && in_letter)
            {
                words[w].letters[letter_count].bounds.top_left.x =
                    start_x;
                words[w].letters[letter_count].bounds.top_left.y =
                    y_min;

                words[w].letters[letter_count].bounds.top_right.x =
                    x;
                words[w].letters[letter_count].bounds.top_right.y =
                    y_min;

                words[w].letters[letter_count].bounds.bottom_right.x =
                    x;
                words[w].letters[letter_count].bounds.bottom_right.y =
                    y_max;

                words[w].letters[letter_count].bounds.bottom_left.x =
                    start_x;
                words[w].letters[letter_count].bounds.bottom_left.y =
                    y_max;

                letter_count++;
                in_letter = 0;
            }
        }
    }

}

