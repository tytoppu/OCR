#include "../include/ia1.h"
#include "../include/forward.h"
#include "../include/image.h"
#include "../include/saving_progress.h"
#include "../include/back_propagation.h"
#include <stdlib.h>
#include<unistd.h>



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

    if (!charger_image_bmp_double("assets/image.bmp", pixels,
         img_size, img_size))
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
        double sortie=resaux->couches[nombre_couches-1].neurones[i].activation;
        printf("sortie[%zu] = %f -> %.0f\n", i, sortie, sortie*128);
    }

    /*training*/
    const double learn_rate = 0.5;
    double target[26];
    const int epochs = 500;
    size_t n_samples = 26;
    double **images = malloc(n_samples * sizeof(*images));
    size_t  *labels = malloc(n_samples * sizeof(*labels));
    if (!images || !labels) {
        free(images);
        free(labels);
        destroy_resaux(resaux);
        free(pixels);
        return EXIT_FAILURE;
    }

    for (size_t s = 0; s < n_samples; ++s)
    {
        images[s] = malloc(nombre_pixels * sizeof(**images));
        if (!images[s]) {
            for (size_t n = 0; n < s; n++)
                free(images[s]);
            free(images);
            free(labels);
            destroy_resaux(resaux);
            free(pixels);
            return EXIT_FAILURE;
        }

        char path[64];
        snprintf(path, sizeof(path),
         "assets/lettres/letter_%zu.bmp", s);

        if (!charger_image_bmp_double(path, images[s],
             img_size, img_size))
        {
            fprintf(stderr, "Impossible de charger %s\n", path);
            for (size_t n = 0; n <= s; ++n)
                free(images[n]);
            free(images);
            free(labels);
            free(pixels);
            destroy_resaux(resaux);
            return EXIT_FAILURE;
        }
        labels[s] = s;
    }

    /*list of each letter's test, will be randomized each epoch*/
    size_t *order = malloc(n_samples * sizeof(*order));
    for (size_t i = 0; i < n_samples; ++i)
        order[i] = i;

    for (int e = 0; e < epochs; ++e){
        /* shuffle the order of the samples */
        for (size_t i = n_samples - 1; i > 0; --i){
            size_t j = (size_t)rand() % (i + 1);
            size_t tmp = order[i]; order[i] = order[j]; order[j] = tmp;
        }

        size_t correct = 0;
        double loss = 0.0;
        for (size_t k = 0; k < n_samples; ++k){
            size_t s = order[k];

            /* put the image into the input layer */
            for (size_t i = 0; i < nombre_pixels; ++i)
                resaux->couches[0].neurones[i].activation = images[s][i];

            for (size_t k = 0; k < 26; ++k)
                target[k] = 0.0;
            target[labels[s]] = 1.0;

            forward_resaux(resaux);
            back_propagation(resaux, target);
            update_weights(resaux, learn_rate);

            /*calculates the % of failure SHOULD go down each epoch*/
            loss += mse(resaux, target);
            size_t best = 0;
            for (size_t c = 1; c < 26; ++c)
                if (resaux->couches[2].neurones[c].activation >
                    resaux->couches[2].neurones[best].activation)
                    best = c;
            if (gessed_letter(resaux) == (char)('A' + labels[s]))
                ++correct;
        }

        if(e%10==0)
            printf("epoch %d  mse %f  acc %zu/%zu  last expect: %c  got: %c\n",
                 e, loss / (double)n_samples, correct, n_samples,
                 (char)('A' + labels[order[n_samples - 1]]),
                 gessed_letter(resaux));

    }
    /*fin training*/

    save_ia(resaux, "trained_ia");

    destroy_resaux(resaux);
    free(pixels);
    printf("\n\tEXIT_SUCCESS\n\n");
    for (size_t s = 0; s < n_samples; ++s)
        free(images[s]);
    free(images);
    free(labels);
    free(order);
    return EXIT_SUCCESS;
}
