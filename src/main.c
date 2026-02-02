#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

static int init(void) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

static int create_window_and_renderer(SDL_Window **window, SDL_Renderer **renderer) {
    const int width = 720;
    const int height = 480;
    const char title[] = "Pixel Draw";
    const SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE |
                                  SDL_WINDOW_INPUT_FOCUS |
                                  SDL_WINDOW_HIGH_PIXEL_DENSITY;

    if (!SDL_CreateWindowAndRenderer(title, width, height, flags, window, renderer)) {
        SDL_Log("Failed to create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

static void destroy_window_and_renderer(SDL_Window *window, SDL_Renderer *renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

static void quit(void) {
    SDL_Quit();
}

int main(void) {
    int status = 0;
    int is_running = true;
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Event event;

    status = init();
    if (status != SDL_APP_CONTINUE) {
        return status;
    }

    status = create_window_and_renderer(&window, &renderer);
    if (status != SDL_APP_CONTINUE) {
        return status;
    }

    while (is_running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    is_running = false;
                    break;
                default: ;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    destroy_window_and_renderer(window, renderer);
    quit();
}
