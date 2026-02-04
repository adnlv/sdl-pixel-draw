#include "canvas.h"

SDL_Texture* create_streaming_texture(SDL_Renderer* renderer, const int w, const int h)
{
    return SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h);
}

void init_canvas(SDL_Renderer* renderer, canvas_t* dest)
{
    dest->texture = create_streaming_texture(renderer, CANVAS_MAX_WIDTH, CANVAS_MAX_HEIGHT);
    dest->dimensions = (SDL_Rect){0};
    dest->grid_dimensions = (SDL_Rect){0};
    dest->grid_cell_size = (SDL_Point){0};
}

void destroy_canvas(canvas_t* canvas)
{
    SDL_DestroyTexture(canvas->texture);

    canvas->texture = NULL;
    canvas->dimensions = (SDL_Rect){0};
    canvas->grid_dimensions = (SDL_Rect){0};
    canvas->grid_cell_size = (SDL_Point){0};
}
