#ifndef SDL_PIXEL_DRAW_CANVAS_H
#define SDL_PIXEL_DRAW_CANVAS_H

#include <SDL3/SDL.h>

#define CANVAS_MAX_WIDTH 0xFF
#define CANVAS_MAX_HEIGHT 0xFF

typedef struct canvas canvas_t;

struct canvas
{
    SDL_Rect dimensions;
    SDL_Rect grid_dimensions;
    SDL_Point grid_cell_size;
    SDL_Texture* texture;
};

void init_canvas(SDL_Renderer* renderer, int w, int h, canvas_t* dest);

void destroy_canvas(canvas_t* canvas);

#endif //SDL_PIXEL_DRAW_CANVAS_H
