#ifndef SDL_PIXEL_DRAW_BINARY_STORAGE_H
#define SDL_PIXEL_DRAW_BINARY_STORAGE_H

#include "storage.h"

FILE* open_binary_file(const char* path);

int close_binary_file(FILE* file);

bool save_pixels_to_binary(FILE* file, uint8_t w, uint8_t h, const uint32_t* pixels);

bool read_pixels_from_binary(FILE* file, uint8_t* w, uint8_t* h, uint32_t* pixels);

#endif //SDL_PIXEL_DRAW_BINARY_STORAGE_H
