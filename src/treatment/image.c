#include "image.h"
#include "bitmap.h"
#include "filter.h"
#include "detection_manager.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * normalizes the image
 * cuts it
    * the grid -> the letters of the grid
    * the words -> the letters of the words
 * returns the matching simple_surface -> conversion to surface -> conversion to image 
*/

char* naming_point_bmp(const char* prefix, int x, int y) {
    int len = snprintf(NULL, 0, "%s_%d_%d.bmp", prefix, x, y);
    if (len < 0) return NULL;
    len++;
    char* new_char = malloc(len);  // +1 for '\0'
    if (!new_char) return NULL;

    snprintf(new_char, len, "%s_%d_%d.bmp", prefix, x, y);
    return new_char;
}


void cell_grid_to_bmp(struct cell_grid* cell){
    char* name = NULL;
    for (int y = 0; y<cell->h; y++){
        for (int x = 0; x<cell->w; x++){
            name = naming_point_bmp("result_test/test_image_cutting/grid/cell", x, y);
            save_simple(cell->simple[x][y], name);
            free(name);
        }
    }
}



void word_list_to_bmp(struct word_list* word){
    char* name = NULL;
    struct word_list* current_word = word;
    struct letter_list* current_letter = NULL;
    int y = 0;
    int x = 0;
    while(current_word){
        name = naming_point_bmp("result_test/test_image_cutting/list/words/word", 0, y);
        save_simple(current_word->word, name);
        free(name);

        current_letter = current_word->letters;
        x = 1;
        while(current_letter){
            name = naming_point_bmp("result_test/test_image_cutting/list/letters/letter", x, y);
            save_simple(current_letter->letter, name);
            free(name);

            current_letter = current_letter->next;
            x++;
        }
        current_word = current_word->next;
        y++;
    }
}


void pretreatment(SDL_Surface* surface, double angle){
    Uint8 anchor;
    struct simple_surface* normalized = normalizer(surface, angle, &anchor);

    struct cell_grid* cell = NULL;
    struct word_list* word = NULL;
    run_detection(normalized, &cell, &word, anchor);

    cell_grid_to_bmp(cell);
    word_list_to_bmp(word);

    destroy_simple_surface(normalized);
    destroy_cell_grid(cell);
    destroy_word_list(word);
}

