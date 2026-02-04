#ifndef SDL_PIXEL_DRAW_DRAW_H
#define SDL_PIXEL_DRAW_DRAW_H

#include <SDL3/SDL.h>

typedef struct
{
    SDL_FPoint pos;
    SDL_MouseButtonFlags btn;
} mouse_state_t;

void update_mouse_state(mouse_state_t* dest);

#endif //SDL_PIXEL_DRAW_DRAW_H
