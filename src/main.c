#include "storage/binary_file_storage.h"

static void write(FILE *file) {
    pixel_t pixels[5] = {
        {0x00, 0x00, 0x00, 0xFF},
        {0xFF, 0x00, 0x00, 0xFF},
        {0x00, 0xFF, 0x00, 0xFF},
        {0x00, 0x00, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF},
    };

    const uint8_t width = 5;
    const uint8_t height = 1;

    save_pixels_as_binary_file(pixels, width, height, file);
}

static void read(FILE *file) {
    uint8_t width = 5;
    uint8_t height = 1;
    pixel_t pixels[width * height];

    read_pixels_from_binary_file(pixels, &width, &height, file);

    printf("W:%d H:%d\n", width, height);

    int i = 0;
    for (; i < width * height; ++i) {
        const pixel_t pixel = pixels[i];
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
