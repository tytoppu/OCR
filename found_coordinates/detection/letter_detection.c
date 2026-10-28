
#include <stdlib.h>
#include <stdio.h>

#include "letter_detection.h"

/*
 * Copie les coordonnées des cases de la grille
 * pour les utiliser comme coordonnées des lettres.
 */


int detect_grid_letters(const DetectionData *data,
                        LetterCoordinates **letters,
                        int *letter_count)
{
    int row;
    int column;
    int index;

    if (data == NULL || letters == NULL || letter_count == NULL)
        return 0;

    *letters = NULL;
    *letter_count = 0;

    if (data->cells == NULL ||
        data->grid.rows <= 0 ||
        data->grid.columns <= 0)
        return 0;

    *letter_count = data->grid.rows * data->grid.columns;

    *letters = malloc(*letter_count * sizeof(LetterCoordinates));

    if (*letters == NULL)
    {
        *letter_count = 0;
        return 0;
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

    return 1;
}




//Repère les lettres d'un mot en regroupant les pixels noirs
int detect_word_letters(const unsigned char *image,
                        int width,
                        int height,
                        int stride,
                        DetectedWord *words,
                        int word_count)
{
    int w;

    if (image == NULL || words == NULL ||
        width <= 0 || height <= 0 || stride < width ||
        word_count < 0)
        return 0;

    for (w = 0; w < word_count; w++)
    {
        int x_min = words[w].bounds.top_left.x;
        int y_min = words[w].bounds.top_left.y;
        int x_max = words[w].bounds.top_right.x;
        int y_max = words[w].bounds.bottom_left.y;
        int x;
        int letter_count = 0;
        int in_letter = 0;
        int start_x = 0;

        if (x_min < 0) x_min = 0;
        if (y_min < 0) y_min = 0;
        if (x_max > width) x_max = width;
        if (y_max > height) y_max = height;

        if (x_min >= x_max || y_min >= y_max)
            return 0;

        // Première passe : compter les groupes de pixels noirs.
         
        for (x = x_min; x < x_max; x++)
        {
            int has_black = 0;
            int y;

            for (y = y_min; y < y_max; y++)
            {
                if (image[y * stride + x] < 128)
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
            return 0;

        words[w].letters = malloc(
            letter_count * sizeof(LetterCoordinates));

        if (words[w].letters == NULL)
            return 0;

        words[w].letter_count = letter_count;

        //enregistre les limites horizontales
     
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
                    if (image[y * stride + x] < 128)
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

    return 1;
}

