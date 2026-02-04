#ifndef SDL_PIXEL_DRAW_DRAW_H
#define SDL_PIXEL_DRAW_DRAW_H

#include <SDL3/SDL.h>

#include "core/screen.h"
#include "core/canvas.h"
#include "core/storage.h"

typedef struct {
    SDL_FPoint pos;
    SDL_MouseButtonFlags btn;
} mouse_state_t;

void update_mouse_state(mouse_state_t *dest);

typedef struct {
    screen_t *screen;
    canvas_t *canvas;
    storage_t *storage;
    mouse_state_t mouse;
} state_t;

void init_state(screen_t *screen, canvas_t *canvas, storage_t *storage, state_t *dest);

void destroy_state(state_t *state);

#endif //SDL_PIXEL_DRAW_DRAW_H
