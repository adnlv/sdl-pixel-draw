cmake_minimum_required(VERSION 4.1)
project(sdl_pixel_draw C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendor/SDL_image EXCLUDE_FROM_ALL)

add_executable(sdl_pixel_draw
        src/main.c
        src/core/mouse.c
        src/core/screen.c
        src/core/canvas.c
        src/core/storage.c
        src/core/binary_storage.c)

target_link_libraries(sdl_pixel_draw PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)

add_custom_target(copy_assets COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/copy_assets.cmake)
add_dependencies(sdl_pixel_draw copy_assets)

