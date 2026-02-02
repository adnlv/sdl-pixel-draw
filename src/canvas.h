#ifndef SDL_PIXEL_DRAW_CANVAS_H
#define SDL_PIXEL_DRAW_CANVAS_H

#include <stdint.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} pixel_t;

#define CANVAS_MAX_WIDTH 0xFF
#define CANVAS_MAX_HEIGHT 0xFF

#endif //SDL_PIXEL_DRAW_CANVAS_H
