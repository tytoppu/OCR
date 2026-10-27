#include "image.h"

/*
 * Ce fichier contient toute les fonction de chargement, 
 * et traitement elementaire de surface / image
 */

SDL_Surface* load_image(const char *path){
    SDL_Surface* picure = IMG_Load(path);
    if (!picure)
        errx(EXIT_FAILURE, "IMG_Load: %s",SDL_GetError());

    return picure;
}

void save_image(SDL_Surface* surface, const char *path){
    if (SDL_SaveBMP(surface, path) != 0)
        errx(EXIT_FAILURE, "SDL_SaveBMP: %s", SDL_GetError());
    //SDL_FreeSurface(surface);
}

SDL_Surface* make_chunck(SDL_Surface* surface, int x1, int y1, int x2, int y2){
    if (x2 >= surface->w || y2 >= surface->h)
        err(EXIT_FAILURE, "save_chunck : Input error, invalid x2 or y2");

    SDL_Surface* chunck = SDL_CreateRGBSurfaceWithFormat(0, x2 - x1, y2 - y1, 
            surface->format->BitsPerPixel, surface->format->format);
    if (!chunck) 
        err(EXIT_FAILURE, "SDL_CreateRGBSurfaceWithFormat : %s", SDL_GetError());

    SDL_Rect rectangle = { x1, y1, x2 - x1, y2 - y1 };

    if (SDL_BlitSurface(surface, &rectangle, chunck, NULL) != 0)
        err(EXIT_FAILURE, "SDL_BlitSurface : %s", SDL_GetError());

    return chunck;
}

SDL_Surface* rotate_picture(SDL_Surface* surface, double angle, Uint8 font){
    double theta = angle * M_PI / 180.0;
    double c = cos(theta);
    double s = sin(theta);

    SDL_PixelFormat* form = surface->format;
    int w = surface->w;
    int h = surface->h;
    double cx = w/2.0;
    double cy = h/2.0;
    SDL_Surface* new = SDL_CreateRGBSurfaceWithFormat(0, w, h,
            form->BitsPerPixel, form->format);

    int bpp = new->format->BytesPerPixel;
    int pitch = new->pitch;

    Uint32 pixel;
    int y = 0;
    int x = 0;
    int x_s;
    int y_s;
    while(y < h){
        x = 0;
        while(x < w){

            x_s = (int)round(c * (x - cx) + s * (y - cy) + cx);
            y_s = (int)round(-s * (x - cx) + c * (y - cy) + cy);

            if (x_s >= 0 && x_s < w && y_s >= 0 && y_s < h)
                memcpy(&pixel, 
                        (Uint8 *) surface->pixels + y_s * pitch + x_s * bpp, 
                        bpp);
            else 
                pixel = SDL_MapRGB(form, font, font, font);
            memcpy((Uint8 *) new->pixels + y * pitch + x * bpp, &pixel, bpp);

            x++;
        }
        y++;
    }
    //SDL_FreeSurface(surface);
    return new;

}


/*
SDL_Surface* rotate_picture(SDL_Surface* src, double angle_deg, Uint8 font){

    for (int y = 0; y < h; y++){
        for (int x = 0; x < w; x++){
            // position correspondante dans la source
            double sx =  c * (x - cx) + s * (y - cy) + cx;
            double sy = -s * (x - cx) + c * (y - cy) + cy;
            int ix = (int)round(sx), iy = (int)round(sy);

            Uint32 pixel = white;
            if (ix >= 0 && ix < w && iy >= 0 && iy < h)
                memcpy(&pixel, (Uint8*)src->pixels + iy*src->pitch + ix*bpp, bpp);

            memcpy((Uint8*)dst->pixels + y*dst->pitch + x*bpp, &pixel, bpp);
        }
    }
    return dst;
}
*/
