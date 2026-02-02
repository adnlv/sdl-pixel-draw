#ifndef SDL_PIXEL_DRAW_FILE_STORAGE_H
#define SDL_PIXEL_DRAW_FILE_STORAGE_H

#include <stdio.h>
#include <stdint.h>

#define BUFFER_MAX_WIDTH 255
#define BUFFER_MAX_HEIGHT 255

typedef struct {
    uint8_t r, g, b, a;
} Pixel;

typedef struct {
    uint8_t width;
    uint8_t height;
    Pixel *pixels;
} Buffer;

int open_file_storage(const char* path, FILE **file);
int write_buffer_to_file(FILE *file, Buffer buffer);
int read_buffer_from_file(FILE *file, Buffer *buffer);

#endif //SDL_PIXEL_DRAW_FILE_STORAGE_H