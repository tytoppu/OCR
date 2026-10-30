#include "../include/ai1.h"

double g_random(size_t n_in)
{
    double limit = sqrt(1.0 / n_in);
    return ((double)rand() / RAND_MAX * 2.0 - 1.0) * limit;
}

/* Returns 0 on size_t overflow */
static int size_mul(size_t a, size_t b, size_t *result)
{
    if (a != 0 && b > SIZE_MAX / a)
        return 0;

    *result = a * b;
    return 1;
}

static int size_add(size_t a, size_t b, size_t *result)
{
    if (b > SIZE_MAX - a)
        return 0;

    *result = a + b;
    return 1;
}

struct link *create_link(struct network *network,size_t link_index,double weight,struct neuron *n1,struct neuron *n2,struct link_list *incoming_list,struct link_list *outgoing_list)
{
    if (!network || link_index >= network->nb_links ||!n1 || !n2 || !incoming_list || !outgoing_list) return NULL;

    struct link *link = &network->links[link_index];

    link->input = n1;
    link->output = n2;
    link->weight = weight;

    incoming_list->link = link;
    incoming_list->prev = NULL;
    incoming_list->next = NULL;

    outgoing_list->link = link;
    outgoing_list->prev = NULL;
    outgoing_list->next = NULL;

    return link;
}

static void append_node(struct link_list **head, struct link_list **tail, struct link_list *new_node)
{
    new_node->next = NULL;

    if (!*head)
    {
        new_node->prev = NULL;
        *head = new_node;
        *tail = new_node;
        return ;
    }

    new_node->prev = *tail;
    (*tail)->next = new_node;
    *tail = new_node;
}


void connect_links(struct network *network, size_t source_layer_index)
{
    if (!network || source_layer_index + 1 >= network->nb_layers)
        return;

    struct layer *source = &network->layers[source_layer_index];
    struct layer *destination = &network->layers[source_layer_index + 1];

    const size_t nb_source = source->nb_neurons;
    const size_t nb_destination = destination->nb_neurons;


    size_t links_offset = 0;
    size_t lists_offset = 0;

    for (size_t layer = 0; layer < source_layer_index; ++layer)
    {
        const size_t n1 = network->layers[layer].nb_neurons;
        const size_t n2 = network->layers[layer + 1].nb_neurons;
        const size_t count = n1 * n2;

        links_offset += count;
        lists_offset += 2 * count;
    }

    for (size_t dst = 0; dst < nb_destination; ++dst)
    {
        struct neuron *destination_neuron = &destination->neurons[dst];

        for (size_t src = 0; src < nb_source; ++src)
        {
            struct neuron *source_neuron = &source->neurons[src];

            const size_t position = dst * nb_source + src;
            const size_t link_index = links_offset + position;
            const size_t incoming_list_index = lists_offset + 2 * position;
            const size_t outgoing_list_index = incoming_list_index + 1;

            struct link_list *incoming_list = &network->link_lists[incoming_list_index];
            struct link_list *outgoing_list = &network->link_lists[outgoing_list_index];

            create_link(network, link_index,g_random(nb_source), source_neuron, destination_neuron,incoming_list, outgoing_list);


            append_node(&destination_neuron->all_inputs, &destination_neuron->last_input, incoming_list);

            append_node(&source_neuron->all_outputs, &source_neuron->last_output, outgoing_list);

            destination_neuron->nb_inputs++;
            source_neuron->nb_outputs++;
        }
    }
}

struct network *create_network(size_t nb_layers, const size_t *layer_sizes)
            // creation of the neural network //
{
    if (nb_layers < 2 || !layer_sizes)
        return NULL;

    srand((unsigned int)time(NULL));

    size_t total_neurons = 0;
    size_t total_links = 0;
    size_t total_link_lists = 0;

    /* Compute the sizes before any allocation. */
    for (size_t i = 0; i < nb_layers; ++i)
    {
        if (layer_sizes[i] == 0)
            return NULL;

        if (!size_add(total_neurons, layer_sizes[i], &total_neurons))
            return NULL;

        if (i > 0)
        {
            size_t layer_links = 0;

            if (!size_mul(layer_sizes[i - 1], layer_sizes[i], &layer_links))
                return NULL;

            if (!size_add(total_links, layer_links, &total_links))
                return NULL;


            size_t layer_lists = 0;
            if (!size_mul(layer_links, 2, &layer_lists))
                return NULL;

            if (!size_add(total_link_lists, layer_lists, &total_link_lists))
                return NULL;
        }
    }

    struct network *network = malloc(sizeof(*network));
    if (!network)
        return NULL;

    network->nb_layers = nb_layers;
    network->nb_neurons = total_neurons;
    network->nb_links = total_links;
    network->nb_link_lists = total_link_lists;
    network->layers = NULL;
    network->neurons = NULL;
    network->links = NULL;
    network->link_lists = NULL;

    network->layers = malloc(nb_layers * sizeof(*network->layers));
    if (!network->layers)
        goto error;

    network->neurons = malloc(total_neurons * sizeof(*network->neurons));
    if (!network->neurons)
        goto error;

    network->links = malloc(total_links * sizeof(*network->links));
    if (!network->links)
        goto error;

    network->link_lists = malloc(total_link_lists * sizeof(*network->link_lists));
    if (!network->link_lists)
        goto error;

    size_t neurons_offset = 0;

    for (size_t i = 0; i < nb_layers; ++i)
    {
        struct layer *layer = &network->layers[i];
        layer->nb_neurons = layer_sizes[i];
        layer->neurons = &network->neurons[neurons_offset];

        for (size_t j = 0; j < layer_sizes[i]; ++j)
        {
            struct neuron *neuron = &layer->neurons[j];

            neuron->bias = g_random(nb_layers);
            neuron->z = 0.0f;
            neuron->activation = 0.0f;
            neuron->delta = 0.0f;
            neuron->all_inputs = NULL;
            neuron->last_input = NULL;
            neuron->nb_inputs = 0;
            neuron->all_outputs = NULL;
            neuron->last_output = NULL;
            neuron->nb_outputs = 0;
        }

        neurons_offset += layer_sizes[i];
    }

    for (size_t i = 0; i + 1 < nb_layers; ++i)
        connect_links(network, i);

    return network;

error:
    free(network->link_lists);
    free(network->links);
    free(network->neurons);
    free(network->layers);
    free(network);
    return NULL;
}


void destroy_network(struct network *network) // destroys and frees the neural network
{
    if (!network)
        return;


    free(network->link_lists);
    free(network->links);
    free(network->neurons);
    free(network->layers);
    free(network);
}




