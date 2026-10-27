#include <stdio.h>
#include <stdlib.h>
#include "../include/ia1.h"
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
    const size_t nombre_couches = sizeof(architecture) / sizeof(architecture[0]);
 
    struct resaux *resaux = cree_resaux(nombre_couches, architecture);
    if (!resaux)
    {
        fprintf(stderr, "Erreur lors de la creation du reseau.\n");
        return EXIT_FAILURE;
    } 
    const size_t n_samples = 4;
    const double entrees[4][2] = {
        {0.0, 0.0},
        {0.0, 1.0},
        {1.0, 0.0},
        {1.0, 1.0},
    };
    const double sorties[4] = {1.0, 0.0, 0.0, 1.0};
 
    const double learn_rate = 0.5;
    const int epochs = 20000;
 
    size_t order[4] = {0, 1, 2, 3};
    const size_t last = nombre_couches - 1;
 
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
 
            resaux->couches[0].neurones[0].activation = entrees[s][0];
            resaux->couches[0].neurones[1].activation = entrees[s][1];
 
            double target[1] = { sorties[s] };
 
            forward_resaux(resaux);
            back_propagation(resaux, target);
            update_weights(resaux, learn_rate);
 
            loss += mse(resaux, target);
 
            double out = resaux->couches[last].neurones[0].activation;
            if ((out >= 0.5 ? 1.0 : 0.0) == sorties[s])
                ++correct;
        }
 
        if (e % 1000 == 0)
            printf("epoch %5d  mse %f  acc %zu/%zu\n",
                   e, loss / (double)n_samples, correct, n_samples);
    }
 
    printf("\n A B | sortie  attendu\n");
    for (size_t s = 0; s < n_samples; ++s)
    {
        resaux->couches[0].neurones[0].activation = entrees[s][0];
        resaux->couches[0].neurones[1].activation = entrees[s][1];
        forward_resaux(resaux);
        double out = resaux->couches[last].neurones[0].activation;
        
        printf(" %.0f %.0f | %f  %.0f  -> %s\n",
               entrees[s][0], entrees[s][1], out, sorties[s],
               ((out >= 0.5 ? 1.0 : 0.0) == sorties[s]) ? "OK" : "FAUX");
    }
 
    save_ia(resaux, "trained_xnor");
    destroy_resaux(resaux);
    printf("\n\tEXIT_SUCCESS\n\n");
    return EXIT_SUCCESS;
}
