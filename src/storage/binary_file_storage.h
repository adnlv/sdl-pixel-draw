#ifndef SDL_PIXEL_DRAW_BINARY_FILE_STORAGE_H
#define SDL_PIXEL_DRAW_BINARY_FILE_STORAGE_H

#include <stdio.h>

#include "../canvas.h"

FILE *open_binary_file(const char *filename);

int close_binary_file(FILE *file);

uint16_t save_pixels_as_binary_file(const pixel_t *pixels,
                                    uint8_t width,
                                    uint8_t height,
                                    FILE *file);

uint16_t read_pixels_from_binary_file(pixel_t *pixels,
                                      uint8_t *width,
                                      uint8_t *height,
                                      FILE *file);

#endif //SDL_PIXEL_DRAW_BINARY_FILE_STORAGE_H
