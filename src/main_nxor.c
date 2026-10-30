#include <stdio.h>
#include <stdlib.h>
#include "../include/ai1.h"
#include "../include/forward.h"
#include "../include/saving_progress.h"
#include "../include/back_propagation.h"

int main(void)
{
    const size_t architecture[] = {
        2,
        4,
        1,
    };
    const size_t nb_layers = sizeof(architecture) / sizeof(architecture[0]);

    struct network *network = create_network(nb_layers, architecture);
    if (!network)
    {
        fprintf(stderr, "Error while creating the network.\n");
        return EXIT_FAILURE;
    }
    const size_t n_samples = 4;
    const double inputs[4][2] = {
        {0.0, 0.0},
        {0.0, 1.0},
        {1.0, 0.0},
        {1.0, 1.0},
    };
    const double outputs[4] = {1.0, 0.0, 0.0, 1.0};

    const double learn_rate = 0.5;
    const int epochs = 20000;

    size_t order[4] = {0, 1, 2, 3};
    const size_t last = nb_layers - 1;

    for (int e = 0; e < epochs; ++e)
    {
        for (size_t i = n_samples - 1; i > 0; --i)
        {
            size_t j = (size_t)rand() % (i + 1);
            size_t tmp = order[i];
            order[i] = order[j];
            order[j] = tmp;
        }

        double loss = 0.0;
        size_t correct = 0;

        for (size_t k = 0; k < n_samples; ++k)
        {
            size_t s = order[k];

            network->layers[0].neurons[0].activation = inputs[s][0];
            network->layers[0].neurons[1].activation = inputs[s][1];

            double target[1] = { outputs[s] };

            forward_network(network);
            back_propagation(network, target);
            update_weights(network, learn_rate);

            loss += mse(network, target);

            double out = network->layers[last].neurons[0].activation;
            if ((out >= 0.5 ? 1.0 : 0.0) == outputs[s])
                ++correct;
        }

        if (e % 1000 == 0)
            printf("epoch %5d  mse %f  acc %zu/%zu\n",
                   e, loss / (double)n_samples, correct, n_samples);
    }

    printf("\n A B | output  expected\n");
    for (size_t s = 0; s < n_samples; ++s)
    {
        network->layers[0].neurons[0].activation = inputs[s][0];
        network->layers[0].neurons[1].activation = inputs[s][1];
        forward_network(network);
        double out = network->layers[last].neurons[0].activation;

        printf(" %.0f %.0f | %f  %.0f  -> %s\n",
               inputs[s][0], inputs[s][1], out, outputs[s],
               ((out >= 0.5 ? 1.0 : 0.0) == outputs[s]) ? "OK" : "WRONG");
    }

    save_ai(network, "trained_xnor");
    destroy_network(network);
    printf("\n\tEXIT_SUCCESS\n\n");
    return EXIT_SUCCESS;
}
