
#include "detection_manager.h"
#include "cell_detection.h"
#include "dominant_color.h"
#include "grid_detection.h"
#include "word_list_detection.h"
#include "letter_detection.h"
#include <err.h>
#include <stdlib.h>
#include <string.h>

//Frees all the memory used by DetectionData.
 /*
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





Runs all detection steps
and fills the DetectionData structure.
*
Returns 1 on success, 0 on error.


void run_detection(struct simple_surface* simple, DetectionData *data){
    if (!simple || !data) err(EXIT_FAILURE, "run_detection : wrong input");

    memset(data, 0, sizeof(*data));
    DominantColor color = detect_writing_color(simple);
    int colors;
    if (color == BLACK)
    	colors=0;
    else
    	colors=255;


    detect_grid_bounds(simple,&data->grid,colors);

    detect_grid_dimensions(simple, &data->grid,colors);

    detect_word_list_bounds(simple, &data->grid, &data->word_list,colors);



    
    build_cells(&data->grid, &data->cells);

    data->cell_count = data->grid.rows * data->grid.columns;
	
    detect_words_bounds(simple,&data->word_list,&data->words, &data->word_count,colors);

    detect_grid_letters(data,&data->grid_letters,&data->grid_letter_count);

    detect_word_letters(simple,data->words, data->word_count,colors);

    free_detection_data(data);
}

*/

