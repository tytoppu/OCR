#include "back_propagation.h"


double sigmoid_derivative(double income)
{
    double s = sigmoid(income);
    return s * (1.0 - s);
}


void back_propagation(struct network *network, const double *target)
{
    size_t last = network->nb_layers - 1;

    /* Output layer */
    struct layer *O = &network->layers[last];
    for (size_t j = 0; j < O->nb_neurons; j++)
    {
        struct neuron *n = &O->neurons[j];
        n->delta = (n->activation - target[j]) * sigmoid_derivative(n->z);
    }

    /* Hidden layers, from the end to the start.
       We stop at l = 1: layer 0 is the input, it has no delta. */
    for (size_t l = last - 1; l >= 1; l--)
    {
        struct layer *C = &network->layers[l];
        for (size_t j = 0; j < C->nb_neurons; j++)
        {
            struct neuron *n = &C->neurons[j];
            double s = 0.0;

            /* sum over outgoing links: weight * delta of the next neuron */
            for (struct link_list *it = n->all_outputs; it != NULL; it = it->next)
                s += it->link->weight * it->link->output->delta;

            n->delta = s * sigmoid_derivative(n->z);
        }
    }
}

char guessed_letter(const struct network *r)
{
    const struct layer *out = &r->layers[r->nb_layers - 1];
    size_t best = 0;
    for (size_t k = 1; k < out->nb_neurons; ++k)
        if (out->neurons[k].activation > out->neurons[best].activation)
            best = k;
    return (char)('A' + best);
}

void update_weights(struct network *network, double lr){
    for (size_t l = 1; l < network->nb_layers; l++){
        struct layer *C = &network->layers[l];
        for (size_t j = 0; j < C->nb_neurons; j++){
            struct neuron *n = &C->neurons[j];

            for (struct link_list *it = n->all_inputs; it != NULL; it = it->next)
                it->link->weight -= lr * n->delta * it->link->input->activation;

            n->bias -= lr * n->delta;
        }
    }
}


double mse(const struct network *r, const double *target){
    const struct layer *out = &r->layers[r->nb_layers - 1];
    double s = 0.0;
    for (size_t k = 0; k < out->nb_neurons; ++k) {
        double d = out->neurons[k].activation - target[k];
        s += 0.5 * d * d;
    }
    return s;
}