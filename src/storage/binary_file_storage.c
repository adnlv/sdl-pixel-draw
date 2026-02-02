#include "binary_file_storage.h"

FILE *open_binary_file(const char *filename) {
    return fopen(filename, "wb+");
}

int close_binary_file(FILE *file) {
    return fclose(file);
}

uint16_t save_pixel_buffer_as_binary_file(const buffer_t *buffer, FILE *file) {
    uint16_t bytes = 0;
    fseek(file, 0, SEEK_SET);
    bytes += fwrite(&buffer->width, sizeof(uint8_t), 1, file);
    bytes += fwrite(&buffer->height, sizeof(uint8_t), 1, file);
    bytes += fwrite(buffer->pixels, sizeof(pixel_t), buffer->width * buffer->height, file);
    return bytes;
}

uint16_t read_pixel_buffer_from_binary_file(buffer_t *buffer, FILE *file) {
    uint16_t bytes = 0;
    fseek(file, 0, SEEK_SET);
    bytes += fread(&buffer->width, sizeof(uint8_t), 1, file);
    bytes += fread(&buffer->height, sizeof(uint8_t), 1, file);
    bytes += fread(buffer->pixels, sizeof(pixel_t), buffer->width * buffer->height, file);
    return bytes;
}
