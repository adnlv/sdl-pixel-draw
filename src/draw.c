#include "draw.h"

void update_mouse_state(mouse_state_t *dest) {
    dest->btn = SDL_GetMouseState(&dest->pos.x, &dest->pos.y);
}

static void reset_mouse_state(mouse_state_t *mouse) {
    mouse->pos = (SDL_FPoint){0};
    mouse->btn = 0;
}

void init_state(screen_t *screen, canvas_t *canvas, storage_t *storage, state_t *dest) {
    dest->screen = screen;
    dest->canvas = canvas;
    dest->storage = storage;
    reset_mouse_state(&dest->mouse);
}

void destroy_state(state_t *state) {
    destroy_canvas(state->canvas);
    destroy_storage(state->storage);
    destroy_screen(state->screen);

    state->screen = NULL;
    state->canvas = NULL;
    state->storage = NULL;
    reset_mouse_state(&state->mouse);
}
