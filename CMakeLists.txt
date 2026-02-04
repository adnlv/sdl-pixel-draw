cmake_minimum_required(VERSION 4.1)
project(sdl_pixel_draw C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)

add_executable(sdl_pixel_draw
        src/main.c
        src/draw.c
        src/core/screen.c
        src/core/canvas.c
        src/core/storage.c
        src/core/binary_storage.c)

target_link_libraries(sdl_pixel_draw PRIVATE SDL3::SDL3)
