#include "screen.h"

void init_screen(SDL_Window* window, SDL_Renderer* renderer, screen_t* dest)
{
    dest->window = window;
    dest->renderer = renderer;
}

void destroy_screen(screen_t* screen)
{
    SDL_DestroyRenderer(screen->renderer);
    SDL_DestroyWindow(screen->window);

    screen->renderer = NULL;
    screen->window = NULL;
}
