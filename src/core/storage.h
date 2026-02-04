#ifndef SDL_PIXEL_DRAW_STORAGE_H
#define SDL_PIXEL_DRAW_STORAGE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef bool (*save_pixels_func_t)(FILE *file, uint8_t w, uint8_t h, const uint32_t *pixels);

typedef bool (*read_pixels_func_t)(FILE *file, uint8_t *w, uint8_t *h, uint32_t *pixels);

#endif //SDL_PIXEL_DRAW_STORAGE_H
