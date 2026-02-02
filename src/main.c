#include "storage/binary_file_storage.h"

static void write(FILE *file) {
    pixel_t pixels[2];

    pixel_t pixel_1;
    pixel_1.r = 0x00;
    pixel_1.g = 0xFF;
    pixel_1.b = 0xBB;
    pixel_1.a = 0xFF;

    pixel_t pixel_2;
    pixel_2.r = 0xDD;
    pixel_2.g = 0x00;
    pixel_2.b = 0x17;
    pixel_2.a = 0xFF;

    pixels[0] = pixel_1;
    pixels[1] = pixel_2;

    buffer_t buffer;
    buffer.width = 2;
    buffer.height = 1;
    buffer.pixels = pixels;

    save_pixel_buffer_as_binary_file(&buffer, file);
}

static void read(FILE *file) {
    buffer_t buffer;

    read_pixel_buffer_from_binary_file(&buffer, file);

    printf("W:%d H:%d\n", buffer.width, buffer.height);

    int i = 0;
    for (; i < buffer.width * buffer.height; ++i) {
        const pixel_t pixel = buffer.pixels[i];
        printf("r:%x g:%x b:%x a:%x\n", pixel.r, pixel.g, pixel.b, pixel.a);
    }
}

int main(void) {
    FILE *file = open_binary_file("image.bin");

    write(file);
    read(file);

    close_binary_file(file);
    return 0;
}
