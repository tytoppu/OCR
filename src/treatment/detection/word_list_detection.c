#include "word_list_detection.h"
#include "new_grid_list_detection.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

/* The image contains one byte per pixel:
black = 0 white = 255. */



void detect_words_bounds(struct simple_surface* list,
        struct word_list** word, Uint8 anchor){

    int *col = calloc(list->w, sizeof(int));
    int *row = calloc(list->h, sizeof(int));
    if (!row || !col) errx(EXIT_FAILURE, "detect_words_bounds : calloc");
    where_is_anchor(list, col, row, anchor);
    free(col);

    struct word_list* word_l = NULL;

    //int len_min = list->h * 0.01;
    int y_start = 0;
    //int len = 0;
    int threshold = list->w /100 + 1;
    // 1 when in black mode and 0 otherwise (on the bands)
    int flag = 0;
    int y = 0;




    while(y < list->h){
        if (row[y] < threshold){
            if(flag == 1) {
                if (y - y_start >= 5){
                    struct word_list* new = init_word_list();
                    new->word = mini_scrap(list, 0, y_start, list->w-1, y-1,
                            anchor);
                    //make_chunk(list, 0, y_start, list->w-1, y-1);
                    append_word_list(&word_l, new);
                }
                flag = 0;
                //len = 0;
                }
                //len ++
            }
        else {
            if(flag == 0) {
                y_start = y;
                flag = 1;
            }
        }
        y++;

    }
    if (flag == 1 && y - y_start >= 5){ 
        struct word_list* new = init_word_list();
        new->word = mini_scrap(list, 0, y_start, list->w-1, y - 1, anchor);
        append_word_list(&word_l, new);
    }

    free(row);
    *word = word_l;
}


void detect_word_letters(struct simple_surface* word,
        struct letter_list** letters, Uint8 anchor){
    int *col = calloc(word->w, sizeof(int));
    int *row = calloc(word->h, sizeof(int));
    if (!row || !col) errx(EXIT_FAILURE, "detect_word_letters : calloc");
    where_is_anchor(word, col, row, anchor);
    free(row);

    struct letter_list* letter_l = NULL;

    int x_start = 0;
    int threshold = word->h /100 + 1;
    // 1 when in black mode and 0 otherwise (on the bands)
    int flag = 0;
    int x = 0;
    while(x < word->w){
        if (col[x] < threshold){
            if(flag == 1) {
                if (x - x_start >= 2){
                    struct letter_list* new = init_letter_list();
                    new->letter = mini_scrap(word, x_start, 0, x-1, word->h-1,
                            anchor);
                    append_letter_list(&letter_l, new);
                }
                flag = 0;
            }
        }
        else {
            if(flag == 0) {
                x_start = x;
                flag = 1;
            }
        }
        x++;

    }
    if (flag == 1 && x - x_start >= 2){ 
        struct letter_list* new = init_letter_list();
        new->letter = mini_scrap(word, x_start, 0, x-1, word->h-1, anchor);
        append_letter_list(&letter_l, new);
    }

    free(col);
    *letters = letter_l;
}


void detect_everything_in_list(struct simple_surface* list,
        struct word_list** word_l,Uint8 anchor){
    struct word_list* word = NULL;
    detect_words_bounds(list, &word, anchor);
    struct word_list* current = word;
    while(current){
        detect_word_letters(current->word, &(current->letters), anchor);
        current = current->next;
    }
    *word_l = word;
}
