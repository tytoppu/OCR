#include "word_list_detection.h"
#include "new_grid_list_detection.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

/* The image contains one byte per pixel:
black = 0 white = 255. */



void detect_words_bounds(struct simple_surface* list, struct word_liste** word, Uint8 ancre){

    int *col = calloc(list->w, sizeof(int));
    int *row = calloc(list->h, sizeof(int));
    if (!row || !col) errx(EXIT_FAILURE, "detect_words_bounds : calloc");
    where_is_anchor(list, col, row, ancre);
    free(col);

    struct word_liste* word_l = NULL;

    //int len_min = list->h * 0.01;
    int y_start = 0;
    //int len = 0;
    int seuil = list->w /100 + 1;
    int flag = 0;           // 1 quand est en mode noire et 0 sinon (sur les bande)
    int y = 0;




    while(y < list->h){
        if (row[y] < seuil){
            if(flag == 1) {
                if (y - y_start >= 5){
                    struct word_liste* new = init_word_liste();
                    new->word = mini_scrap(list, 0, y_start, list->w-1, y-1, ancre);
                    //make_chunck(list, 0, y_start, list->w-1, y-1);
                    append_word_liste(&word_l, new);
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
        struct word_liste* new = init_word_liste();
        new->word = mini_scrap(list, 0, y_start, list->w-1, y - 1, ancre);
        append_word_liste(&word_l, new);
    }

    free(row);
    *word = word_l;
}


void detect_word_letters(struct simple_surface* word, struct letre_list** letre, Uint8 ancre){
    int *col = calloc(word->w, sizeof(int));
    int *row = calloc(word->h, sizeof(int));
    if (!row || !col) errx(EXIT_FAILURE, "detect_word_letters : calloc");
    where_is_anchor(word, col, row, ancre);
    free(row);

    struct letre_list* letre_l = NULL;

    int x_start = 0;
    int seuil = word->h /100 + 1;
    int flag = 0;           // 1 quand est en mode noire et 0 sinon (sur les bande)
    int x = 0;
    while(x < word->w){
        if (col[x] < seuil){
            if(flag == 1) {
                if (x - x_start >= 2){
                    struct letre_list* new = init_letre_list();
                    new->letre = mini_scrap(word, x_start, 0, x-1, word->h-1, ancre);
                    append_letre_list(&letre_l, new);
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
        struct letre_list* new = init_letre_list();
        new->letre = mini_scrap(word, x_start, 0, x-1, word->h-1, ancre);
        append_letre_list(&letre_l, new);
    }

    free(col);
    *letre = letre_l;
}


void detect_everything_in_list(struct simple_surface* list, struct word_liste** word_l,Uint8 ancre){
    struct word_liste* word = NULL;
    detect_words_bounds(list, &word, ancre);
    struct word_liste* parcourt = word;
    while(parcourt){
        detect_word_letters(parcourt->word, &(parcourt->letre), ancre);
        parcourt = parcourt->next;
    }
    *word_l = word;
}
