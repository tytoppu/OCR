#include "forward.h"


double sigmoid(double income)
{
    return 1.0/(1.0+exp(-income));
}



void forward(struct neurone *neurone)
{
    double somme = neurone->biai;
    
    struct list_chaine_lien * current = neurone->all_entre;
    while (current)
    {
        somme += current->lien->weight * current->lien->entre->activation;
        current = current->next;
    }
    
    neurone->z = somme;

    neurone->activation = sigmoid(somme);
}

void forward_couche(struct couche * couche)
{
    for (size_t i = 0; i < couche->nombre_neurones; i++)
    {
        forward(&couche->neurones[i]);
    }
}

void forward_resaux(struct resaux *resaux)
{
    for (size_t i = 0; i < resaux->nombre_couches; i++)
    {
        forward_couche(&resaux->couches[i]);
        printf("couche : %zu\t->\tactivation : %f\n", i, (&resaux->couches[i])->neurones->activation);
    }
}







