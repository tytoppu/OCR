#include "../include/image.h"

static uint16_t lire_u16(FILE *f, int *ok)
{
    unsigned char b[2];

    if (fread(b, 1, sizeof(b), f) != sizeof(b))
    {
        *ok = 0;
        return 0;
    }

    return (uint16_t)b[0] | ((uint16_t)b[1] << 8);
}

static uint32_t lire_u32(FILE *f, int *ok)
{
    unsigned char b[4];

    if (fread(b, 1, sizeof(b), f) != sizeof(b))
    {
        *ok = 0;
        return 0;
    }

    return (uint32_t)b[0]
         | ((uint32_t)b[1] << 8)
         | ((uint32_t)b[2] << 16)
         | ((uint32_t)b[3] << 24);
}

static int32_t lire_i32(FILE *f, int *ok)
{
    return (int32_t)lire_u32(f, ok);
}

static int verifier_taille(size_t a, size_t b, size_t *resultat)
{
    if (a != 0 && b > SIZE_MAX / a)
        return 0;

    *resultat = a * b;
    return 1;
}

int charger_image_bmp_double(const char *chemin,
                             double *destination,
                             size_t largeur,
                             size_t hauteur)
{
    if (!chemin || !destination || largeur == 0 || hauteur == 0)
    {
        printf("error : charger_image_double_bmp_double :: init error -> returned 0\n");
        return 0;
    }

    FILE *f = fopen(chemin, "rb");
    if (!f)
    {
        printf("error : charger_image_bmp_double :: fopen error 'file can't be oppened' -> returned 0\n");
        return 0;
    }

    int ok = 1;

    /* BITMAPFILEHEADER */
    const uint16_t signature = lire_u16(f, &ok);
    (void)lire_u32(f, &ok); /* taille fichier */
    (void)lire_u16(f, &ok); /* reserve */
    (void)lire_u16(f, &ok); /* reserve */
    const uint32_t offset_pixels = lire_u32(f, &ok);

    if (!ok || signature != 0x4D42) /* "BM" */
    {
        fclose(f);
        printf("error : charger_image_bmp_double :: image lecture error -> returned 0\n");
        return 0;
    }

    /* DIB header : on attend au minimum BITMAPINFOHEADER (40 octets). */
    const uint32_t taille_header = lire_u32(f, &ok);
    if (!ok || taille_header < 40)
    {
        fclose(f);
        printf("error : charger_image_bmp_double :: size error -> returned 0\n");
        return 0;
    }

    const int32_t largeur_source_i = lire_i32(f, &ok);
    const int32_t hauteur_source_i = lire_i32(f, &ok);
    const uint16_t plans = lire_u16(f, &ok);
    const uint16_t bits_par_pixel = lire_u16(f, &ok);
    const uint32_t compression = lire_u32(f, &ok);

    (void)lire_u32(f, &ok); /* taille image */
    (void)lire_i32(f, &ok); /* resolution X */
    (void)lire_i32(f, &ok); /* resolution Y */
    (void)lire_u32(f, &ok); /* couleurs */
    (void)lire_u32(f, &ok); /* couleurs importantes */
    //printf("%d\n", compression);

    if (!ok || plans != 1 || largeur_source_i <= 0 || hauteur_source_i == 0 ||
        (bits_par_pixel != 24 && bits_par_pixel != 32) || compression != 0)
    {
        fclose(f);
        printf("error : charger_image_bmp_double :: parsing image error -> returned 0\n");
        return 0;
    }

    const size_t largeur_source = (size_t)largeur_source_i;
    const size_t hauteur_source =
        (size_t)(hauteur_source_i < 0 ? -hauteur_source_i : hauteur_source_i);

    /* Alignement BMP : chaque ligne occupe un multiple de 4 octets. */
    const size_t octets_pixel = bits_par_pixel / 8;
    size_t ligne_brute;

    if (!verifier_taille(largeur_source, octets_pixel, &ligne_brute))
    {
        fclose(f);
        printf("error : charger_image_bmp_double :: size_verification_after_call(0) error -> returned 0\n");
        return 0;
    }

    const size_t ligne = (ligne_brute + 3u) & ~((size_t)3u);

    size_t taille_pixels;
    if (!verifier_taille(ligne, hauteur_source, &taille_pixels))
    {
        fclose(f);
        printf("error : charger_image_bmp_double :: size_verification_after_call(1) error -> returned 0\n");
        return 0;
    }

    unsigned char *pixels = malloc(taille_pixels);
    if (!pixels)
    {
        fclose(f);
        printf("error : charger_image_bmp_double :: pixels error -> returned 0\n");
        return 0;
    }

    if (fseek(f, (long)offset_pixels, SEEK_SET) != 0 ||
        fread(pixels, 1, taille_pixels, f) != taille_pixels)
    {
        free(pixels);
        fclose(f);
        printf("error : charger_image_bmp_double :: fread / fseek error -> returned 0\n");
        return 0;
    }

    fclose(f);

    /*
     * Le BMP peut être bottom-up (hauteur positive) ou top-down (negative).
     * On redimensionne directement vers destination, sans buffer intermediaire.
     *
     * Redimensionnement par MIN-POOLING : pour chaque pixel de destination,
     * on examine tout le bloc de pixels source correspondant et on garde
     * le plus sombre. Contrairement au nearest-neighbor (qui saute des
     * pixels et peut faire disparaitre un trait fin selon l'alignement),
     * ceci garantit qu'un detail sombre (ligne de grille, trait fin) dans
     * le bloc source ressort toujours dans le pixel de destination.
     */
    for (size_t y = 0; y < hauteur; ++y)
    {
        /* plage de lignes source couvertes par cette ligne de destination */
        size_t sy_debut = (y * hauteur_source) / hauteur;
        size_t sy_fin = ((y + 1) * hauteur_source) / hauteur;
        if (sy_fin <= sy_debut) sy_fin = sy_debut + 1;
        if (sy_fin > hauteur_source) sy_fin = hauteur_source;

        for (size_t x = 0; x < largeur; ++x)
        {
            /* plage de colonnes source couvertes par ce pixel de destination */
            size_t sx_debut = (x * largeur_source) / largeur;
            size_t sx_fin = ((x + 1) * largeur_source) / largeur;
            if (sx_fin <= sx_debut) sx_fin = sx_debut + 1;
            if (sx_fin > largeur_source) sx_fin = largeur_source;

            double gris_min = 255.0;

            for (size_t sy = sy_debut; sy < sy_fin; ++sy)
            {
                const size_t ligne_source =
                    (hauteur_source_i > 0) ? (hauteur_source - 1 - sy) : sy;

                const unsigned char *ligne_ptr = pixels + ligne_source * ligne;

                for (size_t sx = sx_debut; sx < sx_fin; ++sx)
                {
                    const unsigned char *pixel = ligne_ptr + sx * octets_pixel;

                    /* BMP stocke BGR(A). */
                    const double b = (double)pixel[0];
                    const double g = (double)pixel[1];
                    const double r = (double)pixel[2];

                    /* Conversion standard approximative en luminance. */
                    const double gris = 0.114 * b + 0.587 * g + 0.299 * r;

                    if (gris < gris_min)
                        gris_min = gris;
                }
            }

            destination[y * largeur + x] = gris_min / 255.0;
        }
    }

    free(pixels);
    return 1;
}