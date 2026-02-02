#ifndef SDL_PIXEL_DRAW_PIXEL_H
#define SDL_PIXEL_DRAW_PIXEL_H

#include <stdint.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} pixel_t;

typedef struct {
    uint8_t width;
    uint8_t height;
    pixel_t *pixels;
} buffer_t;

#endif //SDL_PIXEL_DRAW_PIXEL_H
