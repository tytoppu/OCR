#include "image.h"

/* 
    ce fichier est destine a toute les fonction qui servirons au pretraitment 
    des couleur d'une image, pixel par pixel, pour la rendre + lisible par l'ia 
    et nos algorithme de pretraitement (rotation, decoupage...)
*/



/* ========================================
            auxiliaire
======================================== */
/*
Uint8 GrayAt(SDL_PixelFormat *form, Uint8* p){      // inutile ?
        return la valeur du R d'un pixel pointe par p,
        autrement dit ca veleur grayscale

    Uint32 pixel;
    memcpy(&pixel, p, form->BytesPerPixel);

    Uint8 r;
    Uint8 g;
    Uint8 b;

    SDL_GetRGB(pixel, form, &r, &g, &b);
    return r;
}
*/

void contigu (struct simple_surface* simple, int histogram[256], int* lower_pix, int* highter_pix){
    /*
        calcul les valeur lower_pix et highter_pix des pixel
        au CONTIGUITY % d'extremite (changeable ?)
    */

    int seuil = simple->w * simple->h * CONTIGUITY;     // calcul la contiguite

    int i = 0;
    int parcourt_histo = histogram[i];
    while (parcourt_histo <= seuil && i < 255){
        i ++;
        parcourt_histo += histogram[i];
    }
    *lower_pix = i;

    i = 255;
    parcourt_histo = histogram[i];
    while (parcourt_histo <= seuil && i < 255){
        i --;
        parcourt_histo += histogram[i];
    }
    *highter_pix = i;
}


/*
int seuillage(struct simple_surface* simple, int histogram[256]){
        calcul la valeur inferieur de seuillage pour otsu

    int seuil = simple->w * simple->h * 0.5;

    int i = 0;
    int parcourt_histo = histogram[i];
    while (parcourt_histo <= seuil && i < 255){
        i ++;
        parcourt_histo += histogram[i];
    }
    return i;
}
*/

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

    return (Uint8)(0.299 * r + 0.587 * g + 0.114 * b + 0.5);
}



struct simple_surface* start_grayscale(SDL_Surface* surface){
    /*
     * transphorm une image sous forme de surface en struct simple_surface 
     * en metent tout les pixel en nuance de gris
     */

    int w = surface->w;
    int h = surface->h;
    struct simple_surface* grayscale = init_simple_surface(w, h);

    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint32 pixel;

    int x = 0;      // pos sur la largeur (commence a gauche)
    int y = 0;      // pos sur la heuteur (commence en haut)

    while(y < h){
        x = 0;
        while(x < w){
            pixel = get_pixel(surface, x, y);
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            grayscale->gray[y * w + x] = single_grayscale(r, g, b);

            x++;
        }
        y++;
    }
    return grayscale;
}


/* ========================================
            median filter
======================================== */

Uint8 single_median_filter(struct simple_surface* simple, int x, int y){
    /*
     * renvoi la valeur median dans un carre de 3x3 autour 
     * du pixel cible
     */

    int w = simple->w;
    int h = simple->h;
    Uint8* gray = simple->gray;

    int size = 0;                   // nombre de valeur enregistre
    Uint8 array[9];       // tableau trie en ordre croissant avec les valeur grayscale des pixel
    for (int r = 0; r < 9; r++) {
        array[r] = 255;
    }

    insert_array(array, &size, gray[y * w + x]);    // insert la valeur gray du pixel cible pointe par gray dans l'array (en updatent la tail)

    if (x > 0) {
        insert_array(array, &size, gray[y * w + x - 1]);
        if (y > 0) insert_array(array, &size, gray[(y-1) * w + x-1]);
        if (y < h - 1) insert_array(array, &size, gray[(y+1) * w + x-1]);
    }
    if (x < w - 1) {
        insert_array(array, &size, gray[y * w + x+1]);
        if (y > 0)insert_array(array, &size, gray[(y-1) * w + x+1]);
        if (y < h - 1) insert_array(array, &size, gray[(y+1) * w + x+1]);
    }

    if (y > 0) insert_array(array, &size, gray[(y-1) * w + x]);
    if (y < h - 1) insert_array(array, &size, gray[(y+1) * w + x]);

    // return la medianne
    if (size%2 == 0) return (array[size/2 - 1] + array[size/2]) / 2;
    else return array[size/2];
}


struct simple_surface* median_filter(struct simple_surface* simple, int histogram[256]){
    /*
        prend une surface, creer une nouvelle surface vide appeller new, 
        la rempli pixel par pixel avec le median filter, 
        free l'ancienne surface et renvoi la new
    */

    int w = simple->w;
    int h = simple->h;
    struct simple_surface* median = init_simple_surface(w, h);

    Uint8 gray_median;
    int y = 0;
    int x = 0;
    while(y < h){
        x = 0;
        while(x < w){
            gray_median = single_median_filter(simple, x, y);
            histogram[gray_median] ++;
            median->gray[y*w + x] = gray_median;

            x++;
        }
        y++;
    }
    return median;
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

struct simple_surface* contrast(struct simple_surface* simple, int histogram[256]){
    /*
     * contrast tout les pixel d'une simple_surface,
     * celon un histogram, et a CONTIGUITY % de contiguite
     * return une nouvelle struct simple_surface
    */

    int w = simple->w;
    int h = simple->h;
    struct simple_surface* contraster = init_simple_surface(w, h);

    int lower_pix = -1;
    int highter_pix = -1;
    contigu (simple, histogram, &lower_pix, &highter_pix);

    if (highter_pix <= lower_pix) {
        memcpy(contraster->gray, simple->gray, w * h);
        return contraster;
    }

    //for(size_t i = 0; i<256; i++) histogram[i] = 0;
    Uint8 gray;
    int y = 0;
    int x = 0;
    while(y < h){
        x = 0;
        while(x < w){
            gray = single_contrast(simple->gray[y*w + x], lower_pix, highter_pix);
            //histogram[gray]++;
            contraster->gray[y*w + x] = gray;

            x++;
        }
        y++;
    }
    return contraster;
}



/* =======================================
            black_and_white
======================================== */

Uint8 single_black_and_white(Uint8 gray){
    /*
     * return pour un pixel si il doit etre blanc ou noir 
     */

    if (gray < 128) return 0;
    return 255;
}


struct simple_surface* black_and_white(struct simple_surface* simple){
    /*
     * binarise une image
     */

    int w = simple->w;
    int h = simple->h;
    struct simple_surface* bin = init_simple_surface(w, h);

    //int seuil = seuillage(simple, histogram);

    //black_and_white[2] = {0};
    int y = 0;
    int x = 0;
    while(y < h){
        x = 0;
        while(x < w){
            //black_and_white[bin==255] ++;
            bin->gray[y*w + x] = single_black_and_white(simple->gray[y*w + x]);

            x++;
        }
        y++;
    }
    return bin;
}

/* =======================================
            rotate image
======================================== */

/*
int evaluate_rotation(struct simple_surface simple, double angle){
     * renvoi un struct simple_surface 
     * qui est le parametre simple mais rotate. rien n'a etait free

    double theta = angle * M_PI / 180.0;
    double c = cos(theta);
    double s = sin(theta);


    int new_w = (int)round(fabs(simple->w * c) + fabs(simple->h * s)) + 1;
    int new_h = (int)round(fabs(simple->w * s) + fabs(simple->h * c)) + 1;

    struct simple_surface* rotate = init_simple_surface(new_w, new_h);
    if (!rotate) 
        errx(EXIT_FAILURE, "init_simple_surface failure in rotate_picture");

    double cx = simple->w/2.0;
    double cy = simple->h/2.0;
    double cnew_x = new_w/2.0;
    double cnew_y = new_h/2.0;

    Uint8 pixel;
    int y = 0;
    int x = 0;
    int x_s;
    int y_s;
    while(y < new_h){
        x = 0;
        while(x < new_w){

            x_s = (int)round(c * (x - cnew_x) + s * (y - cnew_y) + cx);
            y_s = (int)round(-s * (x - cnew_x) + c * (y - cnew_y) + cy);

            if (x_s >= 0 && x_s < simple->w && y_s >= 0 && y_s < simple->h)
                pixel = simple->gray[y_s * simple->w + x_s];
            else 
                pixel = font;
            rotate->gray[y * new_w + x] = pixel;

            x++;
        }
        y++;
    }
    return rotate;
}
*/

/* ========================================
            MAIN : normalizer
======================================== */


struct simple_surface* normalizer(SDL_Surface *surface){
    /*
     * tout les process de traitement d'une image, 
     * apres avoir etait charge, une surface est traiter
     * puis on finis par retourner un struct simple_surface
     */

    save_picture(surface, "result_test/0.bmp");

    int histogram[256] = {0};

    struct simple_surface* grayscale = start_grayscale(surface);

    struct simple_surface* median = median_filter(grayscale, histogram);
    destroy_simple_surface(grayscale);

    struct simple_surface* contraster = contrast(median, histogram);
    destroy_simple_surface(median);

    struct simple_surface* binary = black_and_white(contraster);
    destroy_simple_surface(contraster);

    if(!contraster) err(EXIT_FAILURE, "normalizer FAILED");
    return binary;
}
