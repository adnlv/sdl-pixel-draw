#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

static void init(void) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("failed to initialize SDL: %s", SDL_GetError());
        exit(SDL_APP_FAILURE);
    }
}

static void quit(SDL_Window *window, SDL_Renderer *renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

static void create_window_and_renderer(SDL_Window **window, SDL_Renderer **renderer) {
    if (!SDL_CreateWindowAndRenderer("Pixel Draw", 720, 480, SDL_WINDOW_INPUT_FOCUS, window, renderer)) {
        SDL_Log("failed to create window and renderer: %s", SDL_GetError());
        exit(SDL_APP_FAILURE);
    }
}

static int handle_event(const SDL_Event *event) {
    switch (event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        default: ;
    }

    return SDL_APP_CONTINUE;
}

int main(int argc, char *argv[]) {
    init();

    SDL_Window *window;
    SDL_Renderer *renderer;

    create_window_and_renderer(&window, &renderer);

    SDL_Event event;
    int is_running = true;
    while (is_running) {
        while (SDL_PollEvent(&event)) {
            if (handle_event(&event) != SDL_APP_CONTINUE) {
                is_running = false;
                break;
            }
        }
    }

    quit(window, renderer);
}
