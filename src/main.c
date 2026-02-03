#include <stdlib.h>
#include <SDL3/SDL.h>

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

static bool is_point_intersects_rect(const SDL_FPoint *const point, const SDL_FRect *const rect) {
    return point->x >= rect->x &&
           point->x <= rect->x + rect->w &&
           point->y >= rect->y &&
           point->y <= rect->y + rect->h;
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
                default:
                    break;
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

        SDL_FRect picked_color_box;
        picked_color_box.x = left_nav.x;
        picked_color_box.y = left_nav.y;
        picked_color_box.w = left_nav.w;
        picked_color_box.h = left_nav.h * 0.2f;

        const SDL_Color white = {0xFF, 0xFF, 0xFF, 0xFF};
        const SDL_Color black = {0, 0, 0, 0xFF};
        const SDL_Color red = {0xFF, 0, 0, 0xFF};
        const SDL_Color green = {0, 0xFF, 0, 0xFF};
        const SDL_Color blue = {0, 0, 0xFF, 0xFF};
        const SDL_Color yellow = {0xFF, 0xFF, 0x00, 0xFF};

        SDL_Color picked_color = white;

        const int palette_length = 6;
        SDL_Color palette[palette_length];
        palette[0] = red;
        palette[1] = green;
        palette[2] = blue;
        palette[3] = yellow;
        palette[4] = white;
        palette[5] = black;

        SDL_SetRenderDrawColor(renderer, white.r >> 3, white.g >> 3, white.b >> 3, white.a);
        SDL_RenderClear(renderer);

        SDL_FRect palette_boxes[palette_length];

        {
            const int palette_boxes_per_row = 2;
            const float palette_box_size = left_nav.w / (float) palette_boxes_per_row;

            SDL_FRect palette_box;
            palette_box.x = left_nav.x;
            palette_box.y = picked_color_box.y + picked_color_box.h + gap;
            palette_box.w = palette_box_size;
            palette_box.h = palette_box_size;

            int i = 0;
            for (; i < palette_length; ++i) {
                SDL_FRect *box = &palette_boxes[i];
                SDL_Color color = palette[i];

                SDL_Point fac;
                fac.x = i % palette_boxes_per_row;
                fac.y = i / palette_boxes_per_row;

                *box = palette_box;
                box->x += box->w * (float) fac.x;
                box->y += box->h * (float) fac.y;

                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                SDL_RenderFillRect(renderer, box);
            }
        }

        SDL_SetRenderDrawColor(renderer, red.r, red.g, red.b, red.a);
        SDL_RenderRect(renderer, &top_nav);
        SDL_SetRenderDrawColor(renderer, picked_color.r, picked_color.g, picked_color.b, picked_color.a);
        SDL_RenderFillRect(renderer, &picked_color_box);
        SDL_SetRenderDrawColor(renderer, green.r, green.g, green.b, green.a);
        SDL_RenderRect(renderer, &left_nav);
        SDL_SetRenderDrawColor(renderer, blue.r, blue.g, blue.b, blue.a);
        SDL_RenderRect(renderer, &draw_box);
        SDL_RenderPresent(renderer);
    }

    destroy_window_and_renderer(window, renderer);
    quit();
}
