#include "word_list_detection.h"

#include <stddef.h>
#include <stdlib.h>

/* L'image contient un octet par pixel :
   noir = 0 blanc = 255. */
#define INK_THRESHOLD 128
#define MAX_BLANK_GAP 1

static int is_ink(const unsigned char *image,
                  int stride,
                  int x,
                  int y)
{
    return image[(size_t)y * (size_t)stride + (size_t)x]
           < INK_THRESHOLD;
}

static void set_rectangle(Rectangle *r,
                          int x_min,
                          int y_min,
                          int x_max,
                          int y_max)
{
    r->top_left.x = x_min;
    r->top_left.y = y_min;

    r->top_right.x = x_max;
    r->top_right.y = y_min;

    r->bottom_right.x = x_max;
    r->bottom_right.y = y_max;

    r->bottom_left.x = x_min;
    r->bottom_left.y = y_max;
}

/* Calcule la boite englobante des pixels sombres d'une zone. */
static int find_bounds_in_region(const unsigned char *image,
                                 int width,
                                 int height,
                                 int stride,
                                 int x_start,
                                 int x_end,
                                 int *min_x,
                                 int *min_y,
                                 int *max_x,
                                 int *max_y,
                                 int *ink_count)
{
    *min_x = width;
    *min_y = height;
    *max_x = -1;
    *max_y = -1;
    *ink_count = 0;

    if (x_start < 0)
        x_start = 0;

    if (x_end >= width)
        x_end = width - 1;

    if (x_start > x_end)
        return 0;

    for (int y = 0; y < height; ++y) {
        for (int x = x_start; x <= x_end; ++x) {
            if (is_ink(image, stride, x, y)) {
                if (x < *min_x)
                    *min_x = x;

                if (x > *max_x)
                    *max_x = x;

                if (y < *min_y)
                    *min_y = y;

                if (y > *max_y)
                    *max_y = y;

                ++(*ink_count);
            }
        }
    }

    return *ink_count > 0;
}

/*
 * Detecte les limites globales de la liste de mots.
 * La liste peut se trouver a gauche ou a droite de la grille.
 */
int detect_word_list_bounds(const unsigned char *image,
                            int width,
                            int height,
                            int stride,
                            const GridCoordinates *grid,
                            WordListCoordinates *word_list)
{
    int lminx, lminy, lmaxx, lmaxy, lcount;
    int rminx, rminy, rmaxx, rmaxy, rcount;
    int grid_left, grid_right;

    if (!image || !grid || !word_list ||
        width <= 0 || height <= 0 || stride < width)
        return 0;

    grid_left = grid->bounds.top_left.x;
    grid_right = grid->bounds.top_right.x;

    if (grid_left < 0 ||
        grid_right >= width ||
        grid_left >= grid_right)
        return 0;

    find_bounds_in_region(image,
                          width, height, stride,
                          0, grid_left - 1,
                          &lminx, &lminy,
                          &lmaxx, &lmaxy,
                          &lcount);

    find_bounds_in_region(image,
                          width, height, stride,
                          grid_right + 1, width - 1,
                          &rminx, &rminy,
                          &rmaxx, &rmaxy,
                          &rcount);

    /* Choisit le cote contenant le plus de pixels sombres. */
    if (lcount == 0 && rcount == 0)
        return 0;

    if (lcount > 0 &&
        (rcount == 0 || lcount >= rcount)) {

        word_list->side = WORD_LIST_LEFT;

        set_rectangle(&word_list->bounds,
                      lminx, lminy, lmaxx, lmaxy);

    } else {

        word_list->side = WORD_LIST_RIGHT;

        set_rectangle(&word_list->bounds,
                      rminx, rminy, rmaxx, rmaxy);
    }

    return 1;
}

/*
 * Detecte les limites de chaque mot.
 *
 * Hypothese :
 * - un mot par ligne ;
 * - une seule colonne de mots ;
 * - les lignes de mots sont separees par du blanc.
 */
int detect_words_bounds(const unsigned char *image,
                        int width,
                        int height,
                        int stride,
                        const WordListCoordinates *word_list,
                        DetectedWord **words,
                        int *word_count)
{
    int x_min, x_max, y_min, y_max;
    int capacity = 0;
    int in_band = 0;
    int band_start = 0;
    int blank_rows = 0;

    DetectedWord *result = NULL;

    if (!words || !word_count)
        return 0;

    *words = NULL;
    *word_count = 0;

    if (!image || !word_list ||
        width <= 0 || height <= 0 || stride < width)
        return 0;

    x_min = word_list->bounds.top_left.x;
    x_max = word_list->bounds.top_right.x;
    y_min = word_list->bounds.top_left.y;
    y_max = word_list->bounds.bottom_left.y;

    if (x_min < 0 || x_max >= width ||
        y_min < 0 || y_max >= height ||
        x_min > x_max || y_min > y_max)
        return 0;

    /*
     * Parcourt les lignes de pixels de la zone.
     * La marge supplementaire permet de fermer
     * correctement la derniere ligne de texte.
     */
    for (int y = y_min;
         y <= y_max + MAX_BLANK_GAP + 1;
         ++y) {

        int row_has_ink = 0;

        if (y <= y_max) {
            for (int x = x_min; x <= x_max; ++x) {
                if (is_ink(image, stride, x, y)) {
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

            if (blank_rows > MAX_BLANK_GAP) {
                int band_end = y - blank_rows;
                int word_min_x = width;
                int word_max_x = -1;

                /*
                 * Recherche les limites horizontales
                 * du mot dans la bande detectee.
                 */
                for (int yy = band_start;
                     yy <= band_end;
                     ++yy) {

                    for (int x = x_min; x <= x_max; ++x) {
                        if (is_ink(image, stride, x, yy)) {
                            if (x < word_min_x)
                                word_min_x = x;

                            if (x > word_max_x)
                                word_max_x = x;
                        }
                    }
                }

                /* Ajoute le mot au tableau dynamique. */
                if (word_max_x >= word_min_x) {

                    if (*word_count == capacity) {
                        int new_capacity =
                            capacity == 0 ? 8 : capacity * 2;

                        DetectedWord *tmp = realloc(
                            result,
                            (size_t)new_capacity * sizeof(*result)
                        );

                        if (!tmp) {
                            free(result);
                            *word_count = 0;
                            return 0;
                        }

                        result = tmp;
                        capacity = new_capacity;
                    }

                    DetectedWord *word =
                        &result[*word_count];

                    set_rectangle(&word->bounds,
                                  word_min_x,
                                  band_start,
                                  word_max_x,
                                  band_end);

                    word->word = NULL;
                    word->letter_count = 0;
                    word->letters = NULL;

                    ++(*word_count);
                }

                in_band = 0;
                blank_rows = 0;
            }
        }
    }

    if (*word_count == 0) {
        free(result);
        return 0;
    }

    *words = result;

    return 1;
}

/* Libere la memoire allouee pour les mots detectes. */
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
