#ifndef IA1_H
#define IA1_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <err.h>
#include <stdint.h>
#include <limits.h>

struct neurone;
struct lien;
struct list_chaine_lien;
struct couche;
struct resaux;


struct lien
{
    double weight;

    struct neurone *entre;
    struct neurone *sortis;
};

struct list_chaine_lien
{
    struct list_chaine_lien *next;
    struct list_chaine_lien *prev;
    struct lien *lien;
};


struct neurone
{
    double biai;
    double z;           /* somme ponderee avant activation */
    double activation;  /* sortis apres activation */
    double delta;       /* erreur locale pour la retropropagation */

    struct list_chaine_lien *all_entre;
    struct list_chaine_lien *last_entre;
    size_t nombre_entre;

    struct list_chaine_lien *all_sortis;
    struct list_chaine_lien *last_sortis;
    size_t nombre_sortis;
};



struct couche
{
    size_t nombre_neurones;
    struct neurone *neurones;
};

struct resaux
{
    size_t nombre_couches;
    struct couche *couches;

    size_t nombre_neurones;
    struct neurone *neurones;

    size_t nombre_liens;
    struct lien *liens;

    size_t nombre_listes_liens;
    struct list_chaine_lien *listes_liens;
};

double g_random(size_t n_in);

struct resaux *cree_resaux(size_t nombre_couches, const size_t *tailles_couches);
void connect_lien(struct resaux *resaux, size_t indice_couche_source);
struct lien *cree_lien(struct resaux *resaux, size_t indice_lien, double weight, struct neurone *n1, struct neurone *n2, struct list_chaine_lien *liste_entrant, struct list_chaine_lien *liste_sortant);

void destroy_resaux(struct resaux *resaux);

#endif
