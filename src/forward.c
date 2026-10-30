#include "../include/forward.h"


double sigmoid(double income)
{
    return 1.0/(1.0+exp(-income));
}



void forward(struct neuron *neuron)
{
    double sum = neuron->bias;

    struct link_list * current = neuron->all_inputs;
    while (current)
    {
        sum += current->link->weight * current->link->input->activation;
        current = current->next;
    }

    neuron->z = sum;

    neuron->activation = sigmoid(sum);
}

void forward_layer(struct layer * layer)
{
    for (size_t i = 0; i < layer->nb_neurons; i++)
    {
        forward(&layer->neurons[i]);
    }
}

void forward_network(struct network *network)
{
    for (size_t i = 1; i < network->nb_layers; i++)
    {
        forward_layer(&network->layers[i]);
        //printf("layer : %zu\t->\tactivation : %f\n", i, (&network->layers[i])->neurons->activation);
    }
}







