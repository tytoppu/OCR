#include "../include/ia1.h"
#include "../include/forward.h"
#include "../include/image.h"
#include "../include/saving_progress.h"



int main(void)
{
    // chaque paramettre equivaux a une couche, le parametre etant le nombre de neurone 
    const size_t img_size = 32;
    const size_t architecture[] = {
        img_size*img_size,
        128,
        26,
    };

    const size_t nombre_couches = sizeof(architecture) / sizeof(architecture[0]);

    //creation du neural network
    struct resaux *resaux = cree_resaux(nombre_couches, architecture);
    if (!resaux)
    {
        fprintf(stderr, "Erreur lors de la creation du reseau.\n");
        return EXIT_FAILURE;
    }
    //load_ia(resaux, "test_save_ia1");

    const size_t nombre_pixels = img_size*img_size;
    double *pixels = malloc(nombre_pixels * sizeof(*pixels));
    if (!pixels)
    {
        destroy_resaux(resaux);
        return EXIT_FAILURE;
    }

    if (!charger_image_bmp_double("assets/image.bmp", pixels, img_size, img_size))
    {
        fprintf(stderr, "Impossible de charger image.bmp\n");
        free(pixels);
        destroy_resaux(resaux);
        return EXIT_FAILURE;
    }

    //activatioon resaux //

    for (size_t i = 0; i < nombre_pixels; ++i)
        resaux->couches[0].neurones[i].activation = pixels[i];

    forward_resaux(resaux);


    // print and returns //


    printf("Couche      : %zu\n", resaux->nombre_couches);
    printf("Neurone     : %zu\n", resaux->nombre_neurones);
    printf("Connexion   : %zu\n", resaux->nombre_liens);

    for (size_t i = 0; i < 8; ++i)
    {
        double sortie = resaux->couches[nombre_couches - 1].neurones[i].activation;
        printf("sortie[%zu] = %f -> %.0f\n", i, sortie, sortie*128);
    }

    save_ia(resaux, "test_save_ia1");

    destroy_resaux(resaux);
    free(pixels);
    printf("\n\tEXIT_SUCCESS\n\n");
    return EXIT_SUCCESS;
}


