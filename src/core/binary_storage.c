#include "binary_storage.h"

FILE *open_binary_file(const char *path) {
    FILE *file = fopen(path, "r+b");
    if (file == NULL) {
        file = fopen(path, "w+b");
    }

    return file;
}

int close_binary_file(FILE *file) {
    return fclose(file);
}

bool save_pixels_to_binary(FILE *file, const uint8_t w, const uint8_t h, const uint32_t *pixels) {
    fseek(file, 0, SEEK_SET);
    if (fwrite(&w, sizeof(uint8_t), 1, file) == 0)
        return false;

    if (fwrite(&h, sizeof(uint8_t), 1, file) == 0)
        return false;

    if (fwrite(pixels, sizeof(uint32_t), w * h, file) == 0)
        return false;

    return true;
}

bool read_pixels_from_binary(FILE *file, uint8_t *w, uint8_t *h, uint32_t *pixels) {
    fseek(file, 0, SEEK_SET);
    if (fread(w, sizeof(uint8_t), 1, file) == 0)
        return false;

    if (fread(h, sizeof(uint8_t), 1, file) == 0)
        return false;

    if (fread(pixels, sizeof(uint32_t), *w * *h, file) == 0)
        return false;

    return true;
}
