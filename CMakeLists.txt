cmake_minimum_required(VERSION 4.1)
project(sdl_pixel_draw C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)

add_executable(sdl_pixel_draw
        src/main.c
        src/storage/binary_file_storage.c)

target_link_libraries(sdl_pixel_draw PRIVATE SDL3::SDL3)
