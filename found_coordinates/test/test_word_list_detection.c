#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structures/coordinates.h"
#include "structures/detection_data.h"
#include "detection/word_list_detection.h"

#define WIDTH 120
#define HEIGHT 60

static void draw_black_rect(unsigned char *image,
                            int stride,
                            int x1, int y1,
                            int x2, int y2)
{
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            image[y * stride + x] = 0;
        }
    }
}

static int check(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "ECHEC : %s\n", message);
        return 0;
    }
    printf("OK : %s\n", message);
    return 1;
}

int main(void)
{
    unsigned char image[WIDTH * HEIGHT];
    GridCoordinates grid = {0};
    WordListCoordinates word_list = {0};
    DetectedWord *words = NULL;
    int word_count = 0;
    int ok = 1;

    // Fond blanc. 
    memset(image, 255, sizeof(image));

    // Grille fictive : zone x = 50 a 90. 
    grid.bounds.top_left.x = 50;
    grid.bounds.top_left.y = 5;
    grid.bounds.top_right.x = 90;
    grid.bounds.top_right.y = 5;
    grid.bounds.bottom_left.x = 50;
    grid.bounds.bottom_left.y = 50;
    grid.bounds.bottom_right.x = 90;
    grid.bounds.bottom_right.y = 50;

    /*
     * Simule deux mots a gauche de la grille.
     * Chaque mot est represente par une bande noire.
     * Mot 1 : x=10..30, y=10..12
     * Mot 2 : x=12..35, y=22..24
     */
    draw_black_rect(image, WIDTH, 10, 10, 30, 12);
    draw_black_rect(image, WIDTH, 12, 22, 35, 24);

    printf("Test de detect_word_list_bounds()...\n");
    int bounds_ok = detect_word_list_bounds(
        image, WIDTH, HEIGHT, WIDTH, &grid, &word_list);

    ok &= check(bounds_ok, "la zone de la liste est detectee");

    if (!bounds_ok) {
        fprintf(stderr, "Impossible de poursuivre les tests.\n");
        return EXIT_FAILURE;
    }

    ok &= check(word_list.side == WORD_LIST_LEFT,
                "la liste est identifiee a gauche de la grille");
    ok &= check(word_list.bounds.top_left.x == 10 &&
                word_list.bounds.top_left.y == 10,
                "le coin superieur gauche vaut (10, 10)");
    ok &= check(word_list.bounds.bottom_right.x == 35 &&
                word_list.bounds.bottom_right.y == 24,
                "le coin inferieur droit vaut (35, 24)");

    printf("\nTest de detect_words_bounds()...\n");
    int words_ok = detect_words_bounds(
        image, WIDTH, HEIGHT, WIDTH, &word_list, &words, &word_count);

    ok &= check(words_ok, "les lignes de mots sont detectees");
    ok &= check(word_count == 2, "deux lignes de mots sont detectees");

    if (words_ok && word_count == 2) {
        ok &= check(words[0].bounds.top_left.x == 10 &&
                    words[0].bounds.top_left.y == 10 &&
                    words[0].bounds.bottom_right.x == 30 &&
                    words[0].bounds.bottom_right.y == 12,
                    "les limites du premier mot sont correctes");

        ok &= check(words[1].bounds.top_left.x == 12 &&
                    words[1].bounds.top_left.y == 22 &&
                    words[1].bounds.bottom_right.x == 35 &&
                    words[1].bounds.bottom_right.y == 24,
                    "les limites du deuxieme mot sont correctes");
    }

    if (words_ok) {
        printf("\nMots detectes : %d\n", word_count);
        for (int i = 0; i < word_count; i++) {
            printf("Mot %d : x=%d..%d, y=%d..%d\n",
                   i + 1,
                   words[i].bounds.top_left.x,
                   words[i].bounds.top_right.x,
                   words[i].bounds.top_left.y,
                   words[i].bounds.bottom_left.y);
        }
    }

    free_detected_words(words, word_count);

    if (ok) {
        printf("\nTous les tests sont reussis.\n");
        return EXIT_SUCCESS;
    }

    printf("\nAu moins un test a echoue.\n");
    return EXIT_FAILURE;
}

