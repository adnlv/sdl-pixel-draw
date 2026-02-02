#include "binary_file_storage.h"

FILE *open_binary_file(const char *const filename) {
    return fopen(filename, "wb+");
}

int close_binary_file(FILE *const file) {
    return fclose(file);
}

uint16_t save_pixel_buffer_as_binary_file(const pixel_t *const pixels,
                                          const uint8_t width,
                                          const uint8_t height,
                                          FILE *const file) {
    uint16_t items = 0;
    fseek(file, 0, SEEK_SET);
    items += fwrite(&width, sizeof(uint8_t), 1, file);
    items += fwrite(&height, sizeof(uint8_t), 1, file);
    items += fwrite(pixels, sizeof(pixel_t), width * height, file);
    return items;
}

uint16_t read_pixel_buffer_from_binary_file(pixel_t *const pixels,
                                            uint8_t *const width,
                                            uint8_t *const height,
                                            FILE *const file) {
    uint16_t items = 0;
    fseek(file, 0, SEEK_SET);
    items += fread(width, sizeof(uint8_t), 1, file);
    items += fread(height, sizeof(uint8_t), 1, file);
    items += fread(pixels, sizeof(pixel_t), *width * *height, file);
    return items;
}
