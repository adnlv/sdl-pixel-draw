#include "file_storage.h"

static void write(FILE *file) {
    Pixel pixels[2];

    Pixel pixel_1;
    pixel_1.r = 0x00;
    pixel_1.g = 0xFF;
    pixel_1.b = 0xBB;
    pixel_1.a = 0xFF;

    Pixel pixel_2;
    pixel_2.r = 0xDD;
    pixel_2.g = 0x00;
    pixel_2.b = 0x17;
    pixel_2.a = 0xFF;

    pixels[0] = pixel_1;
    pixels[1] = pixel_2;

    Buffer buffer;
    buffer.width = 1;
    buffer.height = 2;
    buffer.pixels = pixels;

    write_buffer_to_file(file, buffer);
}

static void read(FILE *file) {
    Buffer buffer;

    read_buffer_from_file(file, &buffer);

    printf("W:%d H:%d\n", buffer.width, buffer.height);

    int i = 0;
    for (; i < buffer.width * buffer.height; ++i) {
        const Pixel pixel = buffer.pixels[i];
        printf("r:%x g:%x b:%x a:%x\n", pixel.r, pixel.g, pixel.b, pixel.a);
    }
}

int main(void) {
    FILE *file;
    open_file_storage("image.bin", &file);

    write(file);
    read(file);

    fclose(file);
    return 0;
}
