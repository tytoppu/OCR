#ifndef AI1_H
#define AI1_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <err.h>
#include <stdint.h>
#include <limits.h>

struct neuron;
struct link;
struct link_list;
struct layer;
struct network;


struct link
{
    double weight;

    struct neuron *input;
    struct neuron *output;
};

struct link_list
{
    struct link_list *next;
    struct link_list *prev;
    struct link *link;
};


struct neuron
{
    double bias;
    double z;           /* weighted sum before activation */
    double activation;  /* output after activation */
    double delta;       /* local error for backpropagation */

    struct link_list *all_inputs;
    struct link_list *last_input;
    size_t nb_inputs;

    struct link_list *all_outputs;
    struct link_list *last_output;
    size_t nb_outputs;
};



struct layer
{
    size_t nb_neurons;
    struct neuron *neurons;
};

struct network
{
    size_t nb_layers;
    struct layer *layers;

    size_t nb_neurons;
    struct neuron *neurons;

    size_t nb_links;
    struct link *links;

    size_t nb_link_lists;
    struct link_list *link_lists;
};

double g_random(size_t n_in);

struct network *create_network(size_t nb_layers, const size_t *layer_sizes);
void connect_links(struct network *network, size_t source_layer_index);
struct link *create_link(struct network *network, size_t link_index,
        double weight, struct neuron *n1, struct neuron *n2,
        struct link_list *incoming_list, struct link_list *outgoing_list);

void destroy_network(struct network *network);

#endif
