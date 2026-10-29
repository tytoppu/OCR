
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "detection_manager.h"

#include "grid_detection.h"
#include "cell_detection.h"
#include "word_list_detection.h"
#include "letter_detection.h"
#include "dominant_color.h"

//Frees all the memory used by DetectionData.
 
void free_detection_data(DetectionData *data)
{
    if (data == NULL)
        return;

    if (data->cells != NULL)
    {
        free_cells(data->cells, data->grid.rows);
        data->cells = NULL;
    }

    free(data->grid_letters);
    data->grid_letters = NULL;
    data->grid_letter_count = 0;

    if (data->words != NULL)
    {
        free_detected_words(data->words, data->word_count);
        data->words = NULL;
    }

    data->word_count = 0;
    data->cell_count = 0;
}




/*
Runs all detection steps
and fills the DetectionData structure.
*
Returns 1 on success, 0 on error.
*/

int run_detection(const unsigned char *image,
                  int width,
                  int height,
                  int stride,
               
                  DetectionData *data)
{
    if (image == NULL || data == NULL ||
        width <= 0 || height <= 0 || stride < width )
    {
        return -1;
    }

    memset(data, 0, sizeof(*data));
    DominantColor color = detect_dominant_color(image,width,height,stride);
    int colors;
    if (color == BLACK)
    	colors=0;
    else
    	colors=255;


    if (!detect_grid_bounds(image, width, height, stride,
                            &data->grid,colors))
    {
        free_detection_data(data);
        return 2;
    }

    if (!detect_grid_dimensions(image, width, height, stride, &data->grid,colors))
    	return 3;
    if (!detect_word_list_bounds(image, width, height, stride,
                                 &data->grid, &data->word_list,colors))
    {
        free_detection_data(data);
        return 4;
    }

    if (!build_cells(&data->grid, &data->cells))
    {
        free_detection_data(data);
        return 5;
    }
    data->cell_count = data->grid.rows * data->grid.columns;
	
    if (!detect_words_bounds(image, width, height, stride,
                             &data->word_list,
                             &data->words, &data->word_count,colors))
    {
        free_detection_data(data);
        return 6;
    }

    if (!detect_grid_letters(data,
                             &data->grid_letters,
                             &data->grid_letter_count))
    {
        free_detection_data(data);
        return 7;
    }

    if (!detect_word_letters(image, width, height, stride,
                             data->words, data->word_count,colors))
    {
        free_detection_data(data);
        return 8;
    }

    return 1;
}



