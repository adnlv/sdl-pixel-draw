#ifndef SDL_PIXEL_DRAW_STORAGE_H
#define SDL_PIXEL_DRAW_STORAGE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct storage storage_t;

typedef FILE*(*open_file_stream_func_t)(const char* path);

typedef int (*close_file_stream_func_t)(FILE* file);

typedef bool (*save_pixels_func_t)(FILE* file, uint8_t w, uint8_t h, const uint32_t* pixels);

typedef bool (*read_pixels_func_t)(FILE* file, uint8_t* w, uint8_t* h, uint32_t* pixels);

struct storage
{
    FILE* stream;
    open_file_stream_func_t open_file_stream;
    close_file_stream_func_t close_file_stream;
    save_pixels_func_t save_pixels;
    read_pixels_func_t read_pixels;
};

void init_storage(storage_t* dest,
                  open_file_stream_func_t open_file_stream,
                  close_file_stream_func_t close_file_stream,
                  save_pixels_func_t save_pixels,
                  read_pixels_func_t read_pixels);

void destroy_storage(storage_t* storage);

#endif //SDL_PIXEL_DRAW_STORAGE_H
