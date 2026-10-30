#include "ai1.h"
#include "forward.h"
#include "ancient_image.h"
#include "saving_progress.h"
#include "back_propagation.h"
#include <stdlib.h>
#include<unistd.h>



int main(void)
{
    // each parameter is a layer, the value being its number of neurons
    const size_t img_size = 32;
    const size_t architecture[] = {
        img_size*img_size,
        128,
        26,
    };

    const size_t nb_layers = sizeof(architecture) / sizeof(architecture[0]);

    //creation of the neural network
    struct network *network = create_network(nb_layers, architecture);
    if (!network)
    {
        fprintf(stderr, "Error while creating the network.\n");
        return EXIT_FAILURE;
    }
    //load_ai(network, "test_save_ai1");

    const size_t nb_pixels = img_size*img_size;
    double *pixels = malloc(nb_pixels * sizeof(*pixels));
    if (!pixels)
    {
        destroy_network(network);
        return EXIT_FAILURE;
    }

    if (!load_bmp_image_double("assets/image.bmp", pixels,
         img_size, img_size))
    {
        fprintf(stderr, "Unable to load image.bmp\n");
        free(pixels);
        destroy_network(network);
        return EXIT_FAILURE;
    }

    //network activation //

    for (size_t i = 0; i < nb_pixels; ++i)
        network->layers[0].neurons[i].activation = pixels[i];

    forward_network(network);


    // print and returns //


    printf("Layers      : %zu\n", network->nb_layers);
    printf("Neurons     : %zu\n", network->nb_neurons);
    printf("Links       : %zu\n", network->nb_links);

    for (size_t i = 0; i < 8; ++i)
    {
        double output=network->layers[nb_layers-1].neurons[i].activation;
        printf("output[%zu] = %f -> %.0f\n", i, output, output*128);
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
        destroy_network(network);
        free(pixels);
        return EXIT_FAILURE;
    }

    for (size_t s = 0; s < n_samples; ++s)
    {
        images[s] = malloc(nb_pixels * sizeof(**images));
        if (!images[s]) {
            for (size_t n = 0; n < s; n++)
                free(images[s]);
            free(images);
            free(labels);
            destroy_network(network);
            free(pixels);
            return EXIT_FAILURE;
        }

        char path[64];
        snprintf(path, sizeof(path),
         "assets/letters/letter_%zu.bmp", s);

        if (!load_bmp_image_double(path, images[s],
             img_size, img_size))
        {
            fprintf(stderr, "Unable to load %s\n", path);
            for (size_t n = 0; n <= s; ++n)
                free(images[n]);
            free(images);
            free(labels);
            free(pixels);
            destroy_network(network);
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
            for (size_t i = 0; i < nb_pixels; ++i)
                network->layers[0].neurons[i].activation = images[s][i];

            for (size_t k = 0; k < 26; ++k)
                target[k] = 0.0;
            target[labels[s]] = 1.0;

            forward_network(network);
            back_propagation(network, target);
            update_weights(network, learn_rate);

            /*calculates the % of failure SHOULD go down each epoch*/
            loss += mse(network, target);
            size_t best = 0;
            for (size_t c = 1; c < 26; ++c)
                if (network->layers[2].neurons[c].activation >
                    network->layers[2].neurons[best].activation)
                    best = c;
            if (guessed_letter(network) == (char)('A' + labels[s]))
                ++correct;
        }

        if(e%10==0)
            printf("epoch %d  mse %f  acc %zu/%zu  last expect: %c  got: %c\n",
                 e, loss / (double)n_samples, correct, n_samples,
                 (char)('A' + labels[order[n_samples - 1]]),
                 guessed_letter(network));

    }
    /*end of training*/

    save_ai(network, "trained_ai");

    destroy_network(network);
    free(pixels);
    printf("\n\tEXIT_SUCCESS\n\n");
    for (size_t s = 0; s < n_samples; ++s)
        free(images[s]);
    free(images);
    free(labels);
    free(order);
    return EXIT_SUCCESS;
}
