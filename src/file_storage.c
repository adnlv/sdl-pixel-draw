#include "file_storage.h"

int open_file_storage(const char *path, FILE **file) {
    *file = fopen(path, "wb+");
    if (*file == NULL) {
        return -1;
    }

    return 0;
}

int write_buffer_to_file(FILE *file, const Buffer buffer) {
    fwrite(&buffer.width, sizeof(buffer.width), 1, file);
    fwrite(&buffer.height, sizeof(buffer.height), 1, file);
    fwrite(buffer.pixels, sizeof(Pixel), buffer.width * buffer.height, file);
    return 0;
}

int read_buffer_from_file(FILE *file, Buffer *buffer) {
    fread(&buffer->width, sizeof(buffer->width), 1, file);
    fread(&buffer->height, sizeof(buffer->height), 1, file);
    fread(buffer->pixels, sizeof(Pixel), buffer->width * buffer->height, file);
    return 0;
}
