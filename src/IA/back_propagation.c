#include "back_propagation.h"


double derive_sigmoid(double income)
{
    double s = sigmoid(income);
    return s * (1.0 - s);
}


void back_propagation(struct resaux *resaux, const double *target)
{
    size_t last = resaux->nombre_couches - 1;

    /* Couche de sortie */
    struct couche *O = &resaux->couches[last];
    for (size_t j = 0; j < O->nombre_neurones; j++)
    {
        struct neurone *n = &O->neurones[j];
        n->delta = (n->activation - target[j]) * derive_sigmoid(n->z);
    }

    /* Couches cachees, de la fin vers le debut.
       On s'arrete a l = 1 : la couche 0 est l'entree, elle n'a pas de delta. */
    for (size_t l = last - 1; l >= 1; l--)
    {
        struct couche *C = &resaux->couches[l];
        for (size_t j = 0; j < C->nombre_neurones; j++)
        {
            struct neurone *n = &C->neurones[j];
            double s = 0.0;

            /* somme sur les liens sortants : poids * delta du neurone suivant */
            for (struct list_chaine_lien *it = n->all_sortis; it != NULL; it = it->next)
                s += it->lien->weight * it->lien->sortis->delta;

            n->delta = s * derive_sigmoid(n->z);
        }
    }
}
