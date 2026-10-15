#include "../include/ia1.h"

double g_random(size_t n_in) 
{ 
    double limite = sqrt(1.0 / n_in);
    return ((double)rand() / RAND_MAX * 2.0 - 1.0) * limite;
}

/* Retourne 0 en cas d'overflow de size_t */
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

struct lien *cree_lien(struct resaux *resaux,size_t indice_lien,double weight,struct neurone *n1,struct neurone *n2,struct list_chaine_lien *liste_entrant,struct list_chaine_lien *liste_sortant)
{
    if (!resaux || indice_lien >= resaux->nombre_liens ||!n1 || !n2 || !liste_entrant || !liste_sortant) return NULL;

    struct lien *lien = &resaux->liens[indice_lien];

    lien->entre = n1;
    lien->sortis = n2;
    lien->weight = weight;

    liste_entrant->lien = lien;
    liste_entrant->prev = NULL;
    liste_entrant->next = NULL;

    liste_sortant->lien = lien;
    liste_sortant->prev = NULL;
    liste_sortant->next = NULL;

    return lien;
}

static void ajoute_maillon(struct list_chaine_lien **tete, struct list_chaine_lien **queue, struct list_chaine_lien *nouveau)
{
    nouveau->next = NULL;

    if (!*tete)
    {
        nouveau->prev = NULL;
        *tete = nouveau;
        *queue = nouveau;
        return ;
    }

    nouveau->prev = *queue;
    (*queue)->next = nouveau;
    *queue = nouveau;
}


void connect_lien(struct resaux *resaux, size_t indice_couche_source)
{
    if (!resaux || indice_couche_source + 1 >= resaux->nombre_couches)
        return;

    struct couche *source = &resaux->couches[indice_couche_source];
    struct couche *destination = &resaux->couches[indice_couche_source + 1];

    const size_t nombre_source = source->nombre_neurones;
    const size_t nombre_destination = destination->nombre_neurones;

    
    size_t offset_liens = 0;
    size_t offset_listes = 0;

    for (size_t couche = 0; couche < indice_couche_source; ++couche)
    {
        const size_t n1 = resaux->couches[couche].nombre_neurones;
        const size_t n2 = resaux->couches[couche + 1].nombre_neurones;
        const size_t nombre = n1 * n2;

        offset_liens += nombre;
        offset_listes += 2 * nombre;
    }

    for (size_t dst = 0; dst < nombre_destination; ++dst)
    {
        struct neurone *neurone_destination = &destination->neurones[dst];

        for (size_t src = 0; src < nombre_source; ++src)
        {
            struct neurone *neurone_source = &source->neurones[src];

            const size_t position = dst * nombre_source + src;
            const size_t indice_lien = offset_liens + position;
            const size_t indice_liste_entrant = offset_listes + 2 * position;
            const size_t indice_liste_sortant = indice_liste_entrant + 1;

            struct list_chaine_lien *liste_entrant = &resaux->listes_liens[indice_liste_entrant];
            struct list_chaine_lien *liste_sortant = &resaux->listes_liens[indice_liste_sortant];

            cree_lien(resaux, indice_lien,g_random(nombre_source), neurone_source, neurone_destination,liste_entrant, liste_sortant);

                      
            ajoute_maillon(&neurone_destination->all_entre, &neurone_destination->last_entre, liste_entrant);

            ajoute_maillon(&neurone_source->all_sortis, &neurone_source->last_sortis, liste_sortant);

            neurone_destination->nombre_entre++;
            neurone_source->nombre_sortis++;
        }
    }
}

struct resaux *cree_resaux(size_t nombre_couches, const size_t *tailles_couches)
            // creation du resaux de neurone //
{
    if (nombre_couches < 2 || !tailles_couches)
        return NULL;

    srand((unsigned int)time(NULL));

    size_t total_neurones = 0;
    size_t total_liens = 0;
    size_t total_listes_liens = 0;

    /* Calcul des tailles avant toute allocation. */
    for (size_t i = 0; i < nombre_couches; ++i)
    {
        if (tailles_couches[i] == 0)
            return NULL;

        if (!size_add(total_neurones, tailles_couches[i], &total_neurones))
            return NULL;

        if (i > 0)
        {
            size_t liens_couche = 0;

            if (!size_mul(tailles_couches[i - 1], tailles_couches[i], &liens_couche))
                return NULL;

            if (!size_add(total_liens, liens_couche, &total_liens))
                return NULL;

                
            size_t listes_couche = 0;
            if (!size_mul(liens_couche, 2, &listes_couche))
                return NULL;

            if (!size_add(total_listes_liens, listes_couche, &total_listes_liens))
                return NULL;
        }
    }

    struct resaux *resaux = malloc(sizeof(*resaux));
    if (!resaux)
        return NULL;

    resaux->nombre_couches = nombre_couches;
    resaux->nombre_neurones = total_neurones;
    resaux->nombre_liens = total_liens;
    resaux->nombre_listes_liens = total_listes_liens;
    resaux->couches = NULL;
    resaux->neurones = NULL;
    resaux->liens = NULL;
    resaux->listes_liens = NULL;

    resaux->couches = malloc(nombre_couches * sizeof(*resaux->couches));
    if (!resaux->couches)
        goto erreur;

    resaux->neurones = malloc(total_neurones * sizeof(*resaux->neurones));
    if (!resaux->neurones)
        goto erreur;

    resaux->liens = malloc(total_liens * sizeof(*resaux->liens));
    if (!resaux->liens)
        goto erreur;

    resaux->listes_liens = malloc(total_listes_liens * sizeof(*resaux->listes_liens));
    if (!resaux->listes_liens)
        goto erreur;

    size_t offset_neurones = 0;

    for (size_t i = 0; i < nombre_couches; ++i)
    {
        struct couche *couche = &resaux->couches[i];
        couche->nombre_neurones = tailles_couches[i];
        couche->neurones = &resaux->neurones[offset_neurones];

        for (size_t j = 0; j < tailles_couches[i]; ++j)
        {
            struct neurone *neurone = &couche->neurones[j];

            neurone->biai = g_random(nombre_couches);
            neurone->z = 0.0f;
            neurone->activation = 0.0f;
            neurone->delta = 0.0f;
            neurone->all_entre = NULL;
            neurone->last_entre = NULL;
            neurone->nombre_entre = 0;
            neurone->all_sortis = NULL;
            neurone->last_sortis = NULL;
            neurone->nombre_sortis = 0;
        }

        offset_neurones += tailles_couches[i];
    }

    for (size_t i = 0; i + 1 < nombre_couches; ++i)
        connect_lien(resaux, i);

    return resaux;

erreur:
    free(resaux->listes_liens);
    free(resaux->liens);
    free(resaux->neurones);
    free(resaux->couches);
    free(resaux);
    return NULL;
}
            

void destroy_resaux(struct resaux *resaux) // destruction et free du resaux de neurone 
{
    if (!resaux)
        return;

        
    free(resaux->listes_liens);
    free(resaux->liens);
    free(resaux->neurones);
    free(resaux->couches);
    free(resaux);
}




