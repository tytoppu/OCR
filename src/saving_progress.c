#include "../include/saving_progress.h"

void load_ia(struct resaux *resaux, char *filename)
{
    if (!filename || !resaux) errx(EXIT_FAILURE, "load_ai : n'y a pas de resaux ou pas de fichier\n");

    FILE *f = fopen(filename, "rb");
    if (!f) errx(EXIT_FAILURE, "load_ia : can't oppen the file '%s'\n", filename);


    double *biai = malloc(resaux->nombre_neurones * sizeof(double));
    double *weight = malloc(resaux->nombre_liens * sizeof(double));

    if (fread(biai, sizeof(double), resaux->nombre_neurones, f) != resaux->nombre_neurones ||
        fread(weight, sizeof(double), resaux->nombre_liens, f)  != resaux->nombre_liens)
        errx(EXIT_FAILURE, "load_ia : can't read the file %s", filename);
    
    for (size_t i = 0; i < resaux->nombre_neurones; i++)
        resaux->neurones[i].biai = biai[i];
    
    for (size_t i = 0; i < resaux->nombre_liens; i++)
        resaux->liens[i].weight = weight[i];

    free(biai);
    free(weight);
    fclose(f);
}


void save_ia(struct resaux *resaux, char *filename)
{
    if (!filename || !resaux) errx(EXIT_FAILURE, "save_ia : il n'y a pas de resaux(%p) ou de fichier(%p)\n", resaux, filename);

    FILE *f = fopen(filename, "wb");
    if (!f) errx(EXIT_FAILURE, "save_ia : can't oppen the file to save the progress\n");


    double *biai = malloc(resaux->nombre_neurones * sizeof(double));
    double *weight = malloc(resaux->nombre_liens * sizeof(double));

    for (size_t i = 0; i< resaux->nombre_neurones; i++)
        biai[i] = resaux->neurones[i].biai;

    for (size_t i = 0; i<resaux->nombre_liens; i++)
        weight[i] = resaux->liens[i].weight;

    if (fwrite(biai, resaux->nombre_neurones, sizeof(double), f) == 0 || 
        fwrite(weight, resaux->nombre_liens, sizeof(double), f) == 0)
        errx(EXIT_FAILURE, "save_ia : can't write on the file %s\n", filename);
    
    free(biai);
    free(weight);
    fclose(f);
}