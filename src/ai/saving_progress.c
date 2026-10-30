#include "saving_progress.h"

void load_ai(struct network *network, char *filename)
{
    if (!filename || !network) errx(EXIT_FAILURE, "load_ai : there is no network or no file\n");

    FILE *f = fopen(filename, "rb");
    if (!f) errx(EXIT_FAILURE, "load_ai : can't open the file '%s'\n", filename);


    double *bias = malloc(network->nb_neurons * sizeof(double));
    double *weight = malloc(network->nb_links * sizeof(double));

    if (fread(bias, sizeof(double), network->nb_neurons, f) != network->nb_neurons ||
        fread(weight, sizeof(double), network->nb_links, f)  != network->nb_links)
        errx(EXIT_FAILURE, "load_ai : can't read the file %s", filename);

    for (size_t i = 0; i < network->nb_neurons; i++)
        network->neurons[i].bias = bias[i];

    for (size_t i = 0; i < network->nb_links; i++)
        network->links[i].weight = weight[i];

    free(bias);
    free(weight);
    fclose(f);
}


void save_ai(struct network *network, char *filename)
{
    if (!filename || !network) errx(EXIT_FAILURE, "save_ai : there is no network(%p) or no file(%p)\n", network, filename);

    FILE *f = fopen(filename, "wb");
    if (!f) errx(EXIT_FAILURE, "save_ai : can't open the file to save the progress\n");


    double *bias = malloc(network->nb_neurons * sizeof(double));
    double *weight = malloc(network->nb_links * sizeof(double));

    for (size_t i = 0; i< network->nb_neurons; i++)
        bias[i] = network->neurons[i].bias;

    for (size_t i = 0; i<network->nb_links; i++)
        weight[i] = network->links[i].weight;

    if (fwrite(bias, network->nb_neurons, sizeof(double), f) == 0 ||
        fwrite(weight, network->nb_links, sizeof(double), f) == 0)
        errx(EXIT_FAILURE, "save_ai : can't write on the file %s\n", filename);

    free(bias);
    free(weight);
    fclose(f);
}
