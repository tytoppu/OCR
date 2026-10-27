#include "image.h"

/* 
    ce fichier est destine a toute les fonction qui servirons au pretraitment 
    des couleur d'une image, pixel par pixel, pour la rendre + lisible par l'ia 
    et nos algorithme de pretraitement (rotation, decoupage...)
*/



/* ========================================
            auxiliaire
======================================== */

Uint8 GrayAt(SDL_PixelFormat *form, Uint8* p){
    /*
        return la valeur du R d'un pixel pointe par p,
        autrement dit ca veleur grayscale
    */
    Uint32 pixel;
    memcpy(&pixel, p, form->BytesPerPixel);

    Uint8 r;
    Uint8 g;
    Uint8 b;

    SDL_GetRGB(pixel, form, &r, &g, &b);
    return r;
}


void contigu (SDL_Surface* surface, int histogram[256], int* lower_pix, int* highter_pix){
    /*
        calcul les valeur lower_pix et highter_pix des pixel
        au 5% d'extremite
    */

    int parcourt_histo = 0;                         // compt le nombre de valeur parcouru
    int contigu = surface->w * surface->h * 0.05;
    int i = 0;
    while (parcourt_histo <= contigu){
        parcourt_histo += histogram[i];
        i ++;
    }
    *lower_pix = i;

    i = 255;
    parcourt_histo = 0;
    while (parcourt_histo <= contigu){
        parcourt_histo += histogram[i];
        i --;
    }
    *highter_pix = i;
}



void insert_array(Uint8 array[9], int* size, Uint8 elem){
    /*
        insert un element dans l'aray de facon a le garder
        trier en ordre croissant
    */

    size_t i = 8;
    while (i > 0 && array[i-1] > elem){
        array[i] = array[i-1];
        i--;
    }
    array[i] = elem;
    (*size) ++;
}



/* ========================================
                grayscale
======================================== */


Uint8 single_grayscale(Uint8 r, Uint8 g, Uint8 b){
    /*
        return la valeur grayscale d'un pixel celon
        son RGB
    */

    return 0.299 * r + 0.587 * g + 0.114 * b;
}



void grayscale(SDL_Surface* surface){
    /*
        transphorm l'image en place en niveaux de gris
    */

    Uint8 *p = NULL;    // pointeur sur pixel
    Uint32 pixel;       // valeur RGB d'un pixel

    int bpp = surface->format->BytesPerPixel;   // vrais tail d'un pixel
    SDL_PixelFormat* form = surface->format;    // format de l'image'
    int x = 0;      // pos sur la largeur (commence a gauche)
    int y = 0;      // pos sur la heuteur (commence en haut)

    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 gray;     // gray final ("moyenne" des RGB enssemble)

    while(y < surface->h){
        x = 0;
        while(x < surface->w){
            p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
            memcpy(&pixel, p, bpp);

            SDL_GetRGB(pixel, form, &r, &g, &b);
            gray = single_grayscale(r, g, b);

            pixel = SDL_MapRGB(form, gray, gray, gray);
            memcpy(p, &pixel, bpp);

            x++;
        }
        y++;
    }
}


/* ========================================
            median filter
======================================== */

Uint8 single_median_filter(SDL_Surface* surface, int x, int y){
    /*
        renvoi la valeur median dans un carre de 3x3 autour 
        du pixel cible
    */
    SDL_PixelFormat* form = surface->format;
    int bpp = form->BytesPerPixel;
    int pitch = surface->pitch;
    int w = surface->w;
    int h = surface->h;

    int size = 0;                   // nombre de valeur enregistre
    Uint8 array[9];       // tableau trie en ordre croissant avec les valeur grayscale des pixel
    for (int r = 0; r < 9; r++) {
        array[r] = 255;
    }

    Uint8* p = (Uint8 *)surface->pixels + y * pitch + x * bpp;
    insert_array(array, &size, GrayAt(form, p));    // insert la valeur gray du pixel cible pointe par p (avec le format form) dans l'array (en updatent la tail)

    if (x > 0) {
        insert_array(array, &size, GrayAt(form,(p - bpp)));
        if (y > 0) insert_array(array, &size, GrayAt(form,(p - bpp - pitch)));
        if (y < h - 1) insert_array(array, &size, GrayAt(form,(p - bpp + pitch)));
    }
    if (x < w - 1) {
        insert_array(array, &size, GrayAt(form,(p + bpp)));
        if (y > 0)insert_array(array, &size, GrayAt(form,(p + bpp - pitch)));
        if (y < h - 1) insert_array(array, &size, GrayAt(form,(p + bpp + pitch)));
    }

    if (y > 0) insert_array(array, &size, GrayAt(form,(p - pitch)));
    if (y < h - 1) insert_array(array, &size, GrayAt(form,(p + pitch)));

    // return la medianne
    //for (size_t k = 0; k<9; k++) printf("%i\n", array[k]);
    //printf("\n\n");
    if (size%2 == 0) return (array[size/2 - 1] + array[size/2]) / 2;
    else return array[size/2];
}


SDL_Surface* median_filter(SDL_Surface *surface, int histogram[256]){
    /*
        prend une surface, creer une nouvelle surface vide appeller new, 
        la rempli pixel par pixel avec le median filter, 
        free l'ancienne surface et renvoi la new
    */

    SDL_PixelFormat* form = surface->format;
    int w = surface->w;
    int h = surface->h;
    SDL_Surface* new = SDL_CreateRGBSurfaceWithFormat(0, w, h, 
            form->BitsPerPixel, form->format);

    int bpp = new->format->BytesPerPixel;
    int pitch = new->pitch;
    Uint8 gray_median;
    Uint32 pixel;
    Uint8* p = NULL;
    int y = 0;
    int x = 0;
    while(y < h){
        x = 0;
        while(x < w){
            gray_median = single_median_filter(surface, x, y);
            histogram[gray_median] ++;

            pixel = SDL_MapRGB(form, gray_median, gray_median, gray_median);
            p = (Uint8 *) new->pixels + y * pitch + x * bpp;
            memcpy(p, &pixel, bpp);

            x++;
        }
        y++;
    }
    SDL_FreeSurface(surface);
    return new;
}


/* ========================================
                contrast
======================================== */




Uint8 single_contrast(Uint8 gray, Uint8 min, Uint8 max){
    /*
        return la valeur grayscale d'un pixel, 
        apres "contrastage" celon les pixel de 5% min et max
    */

    int answer = (gray - min) * 255 / (max - min);
    if (answer<0) return 0;
    else if (answer>255) return 255;
    return answer;
}

void contrast(SDL_Surface* surface, int histogram[256]){
    /*
        contrast tout les pixel d'une surface, en place, 
        celon un histogram, et a 5% de contiguite
    */

    SDL_PixelFormat* form = surface->format;
    int bpp = form->BytesPerPixel;
    int pitch = surface->pitch;
    int w = surface->w;
    int h = surface->h;

    int lower_pix = -1;
    int highter_pix = -1;
    contigu (surface, histogram, &lower_pix, &highter_pix);

    Uint32 pixel;
    Uint8* p = NULL;

    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 gray;

    int y = 0;
    int x = 0;
    while(y < h){
        x = 0;
        while(x < w){
            p = (Uint8 *)surface->pixels + y * pitch + x * bpp;
            memcpy(&pixel, p, bpp);
            SDL_GetRGB(pixel, form, &r, &g, &b);
            //gray = black_and_white(single_contrast(r, lower_pix, highter_pix));
            gray = single_contrast(r, lower_pix, highter_pix);
            pixel = SDL_MapRGB(form, gray, gray, gray);
            memcpy(p, &pixel, bpp);
            x++;
        }
        y++;
    }
}



/* =======================================
            black_and_white
======================================== */

Uint8 black_and_white(Uint8 gray){
    if (gray < 127) return 0;
    return 255;
}






/* ========================================
            MAIN : normalizer
======================================== */


SDL_Surface* normalizer(SDL_Surface *surface){
    /*
        tout les process de traitement de l'image, 
        qui finisse par retourne une nouvelle surface
    */
    int histogram[256];
    grayscale(surface);

    return surface;
    SDL_Surface* new_surface = median_filter(surface, histogram);

    contrast(new_surface, histogram);

    if(!new_surface) err(EXIT_FAILURE, "normalizer FAILED");
    return new_surface;
}
