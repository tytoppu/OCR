#include "image.h"
#include "bitmap.h"
#include "filter.h"
#include "detection_manager.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * normalise l'image
 * on la decoupe
    * la grille -> les letre de la grille
    * les mots -> les letre des mots
 * on renvoi les simple_surface associer -> conversion en surface -> conversion en image 
*/

char* naming_point_bnp(const char* prefix, int x, int y) {
    int len = snprintf(NULL, 0, "%s_%d_%d.bmp", prefix, x, y);
    if (len < 0) return NULL;
    len++;
    char* new_char = malloc(len);  // +1 pour '\0'
    if (!new_char) return NULL;

    snprintf(new_char, len, "%s_%d_%d.bmp", prefix, x, y);
    return new_char;
}


void cell_grid_to_bmp(struct cell_grid* cell){
    char* name = NULL;
    for (int y = 0; y<cell->h; y++){
        for (int x = 0; x<cell->w; x++){
            name = naming_point_bnp("result_test/test_image_decouping/grid/cell", x, y);
            save_simple(cell->simple[x][y], name);
            free(name);
        }
    }
}



void word_liste_to_bmp(struct word_liste* word){
    char* name = NULL;
    struct word_liste* parcourt_word = word;
    struct letre_list* parcourt_letre = NULL;
    int y = 0;
    int x = 0;
    while(parcourt_word){
        name = naming_point_bnp("result_test/test_image_decouping/list/words/word", 0, y);
        save_simple(parcourt_word->word, name);
        free(name);

        parcourt_letre = parcourt_word->letre;
        x = 1;
        while(parcourt_letre){
            name = naming_point_bnp("result_test/test_image_decouping/list/leters/leter", x, y);
            save_simple(parcourt_letre->letre, name);
            free(name);

            parcourt_letre = parcourt_letre->next;
            x++;
        }
        parcourt_word = parcourt_word->next;
        y++;
    }
}


void pretreatment(SDL_Surface* surface, double angle){
    Uint8 ancre;
    struct simple_surface* normalized = normalizer(surface, angle, &ancre);

    struct cell_grid* cell = NULL;
    struct word_liste* word = NULL;
    run_detection(normalized, &cell, &word, ancre);

    cell_grid_to_bmp(cell);
    word_liste_to_bmp(word);

    destroy_simple_surface(normalized);
    destroy_cell_grid(cell);
    destroy_word_liste(word);
}

