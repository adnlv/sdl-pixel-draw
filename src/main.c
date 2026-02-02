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

        int out_width = 0;
        int out_height = 0;
        SDL_GetCurrentRenderOutputSize(renderer, &out_width, &out_height);

        const float gap = 4.0f;

        SDL_FRect top_nav;
        const float top_nav_min_height = 40.0f;
        const float top_nav_max_height = 50.0f;
        const float top_nav_scaling_fac = 0.01f;
        top_nav.x = 0;
        top_nav.y = 0;
        top_nav.w = (float) out_width;
        top_nav.h = top_nav_min_height + (float) out_height * top_nav_scaling_fac;
        if (top_nav.h > top_nav_max_height) {
            top_nav.h = top_nav_max_height;
        }

        const float left_nav_min_width = 60.0f;
        const float left_nav_max_width = 160.0f;
        const float left_nav_scaling_fac = 0.08f;
        SDL_FRect left_nav;
        left_nav.x = 0;
        left_nav.y = top_nav.h + gap;
        left_nav.w = left_nav_min_width + (float) out_width * left_nav_scaling_fac;
        if (left_nav.w > left_nav_max_width) {
            left_nav.w = left_nav_max_width;
        }
        left_nav.h = (float) out_height - left_nav.y - gap;

        SDL_FRect draw_box;
        draw_box.x = left_nav.x + left_nav.w + gap;
        draw_box.y = left_nav.y;
        draw_box.w = (float) out_width - draw_box.x - gap;
        draw_box.h = (float) out_height - draw_box.y - gap;

        const SDL_Color black_color = {0, 0, 0, 0xFF};
        const SDL_Color red_color = {0xFF, 0, 0, 0xFF};
        const SDL_Color green_color = {0, 0xFF, 0, 0xFF};
        const SDL_Color blue_color = {0, 0, 0xFF, 0xFF};

        SDL_SetRenderDrawColor(renderer, black_color.r, black_color.g, black_color.b, black_color.a);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, red_color.r, red_color.g, red_color.b, red_color.a);
        SDL_RenderRect(renderer, &top_nav);
        SDL_SetRenderDrawColor(renderer, green_color.r, green_color.g, green_color.b, green_color.a);
        SDL_RenderRect(renderer, &left_nav);
        SDL_SetRenderDrawColor(renderer, blue_color.r, blue_color.g, blue_color.b, blue_color.a);
        SDL_RenderRect(renderer, &draw_box);
        SDL_RenderPresent(renderer);
    }

    destroy_window_and_renderer(window, renderer);
    quit();
}
