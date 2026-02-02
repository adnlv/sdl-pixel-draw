#ifndef SDL_PIXEL_DRAW_BINARY_FILE_STORAGE_H
#define SDL_PIXEL_DRAW_BINARY_FILE_STORAGE_H

#include <stdio.h>
#include <stdint.h>

#include "../pixel.h"

FILE *open_binary_file(const char *filename);

int close_binary_file(FILE *file);

uint16_t save_pixel_buffer_as_binary_file(const buffer_t *buffer, FILE *file);

uint16_t read_pixel_buffer_from_binary_file(buffer_t *buffer, FILE *file);

#endif //SDL_PIXEL_DRAW_BINARY_FILE_STORAGE_H
