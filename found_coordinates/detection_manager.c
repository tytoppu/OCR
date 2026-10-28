
#include <stdlib.h>
#include <string.h>

#include "detection_manager.h"

#include "detection/grid_detection.h"
#include "detection/cell_detection.h"
#include "detection/word_list_detection.h"
#include "detection/letter_detection.h"


//Libere toute la memoire utilisee par DetectionData.
 
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
 Lance toutes les etapes de detection
 et remplit la structure DetectionData.
 *
 Return 1 en cas de succes, 0 en cas d'erreur.
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
        return 0;
    }

    memset(data, 0, sizeof(*data));

    if (!detect_grid_bounds(image, width, height, stride,
                            &data->grid))
    {
        goto error;
    }

    if (!detect_grid_dimensions(image, width, height, stride, &data->grid))
    	return 0;
    if (!detect_word_list_bounds(image, width, height, stride,
                                 &data->grid, &data->word_list))
    {
        goto error;
    }

    if (!build_cells(&data->grid, &data->cells))
    {
        goto error;
    }
    data->cell_count = data->grid.rows * data->grid.columns;
	
    if (!detect_words_bounds(image, width, height, stride,
                             &data->word_list,
                             &data->words, &data->word_count))
    {
        goto error;
    }

    if (!detect_grid_letters(data,
                             &data->grid_letters,
                             &data->grid_letter_count))
    {
        goto error;
    }

    if (!detect_word_letters(image, width, height, stride,
                             data->words, data->word_count))
    {
        goto error;
    }

    return 1;

error:
    free_detection_data(data);
    return 0;
}




