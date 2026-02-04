#ifndef SDL_PIXEL_DRAW_SCREEN_H
#define SDL_PIXEL_DRAW_SCREEN_H

#include <SDL3/SDL_render.h>

typedef struct screen screen_t;

struct screen
{
    SDL_Window* window;
    SDL_Renderer* renderer;
};

void init_screen(SDL_Window* window, SDL_Renderer* renderer, screen_t* dest);

void destroy_screen(screen_t* screen);

#endif //SDL_PIXEL_DRAW_SCREEN_H
