#include "mouse.h"

void update_mouse_state(mouse_state_t* dest)
{
    dest->btn = SDL_GetMouseState(&dest->pos.x, &dest->pos.y);
}
