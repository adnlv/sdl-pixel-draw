#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>

#include "canvas.h"

static const SDL_Color white = {0xFF, 0xFF, 0xFF, 0xFF};
static const SDL_Color black = {0, 0, 0, 0xFF};
static const SDL_Color red = {0xFF, 0, 0, 0xFF};
static const SDL_Color green = {0, 0xFF, 0, 0xFF};
static const SDL_Color blue = {0, 0, 0xFF, 0xFF};
static const SDL_Color yellow = {0xFF, 0xFF, 0x00, 0xFF};
static const SDL_Color magenta = {0xFF, 0x00, 0xFF, 0xFF};
static const SDL_Color cyan = {0x00, 0xFF, 0xFF, 0xFF};

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

    if (!SDL_SetRenderVSync(*renderer, 1)) {
        SDL_Log("Failed to enable VSync: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

static bool is_point_intersects_rect(const SDL_FPoint *const point, const SDL_FRect *const rect) {
    return (int) point->x > (int) rect->x &&
           (int) point->x < (int) (rect->x + rect->w) &&
           (int) point->y > (int) rect->y &&
           (int) point->y < (int) (rect->y + rect->h);
}

static void calculate_render_output_boundaries(SDL_Renderer *renderer, SDL_Rect *const dest) {
    dest->x = 0;
    dest->y = 0;

    SDL_GetCurrentRenderOutputSize(renderer, &dest->w, &dest->h);
}

static void calculate_top_navigation_bar_boundaries(const SDL_Rect *const out, SDL_FRect *const dest) {
    const float min_h = 40.0f;
    const float max_h = 50.0f;
    const float h_scaling_fac = 0.01f;

    dest->x = 0;
    dest->y = 0;
    dest->w = (float) out->w;
    dest->h = min_h + (float) out->h * h_scaling_fac;
    if (dest->h > max_h) {
        dest->h = max_h;
    }
}

static void calculate_left_navigation_bar_boundaries(const SDL_Rect *const out,
                                                     const SDL_FRect *const top_navigation_bar,
                                                     const float gap,
                                                     SDL_FRect *const dest) {
    const float min_w = 60.0f;
    const float max_w = 160.0f;
    const float w_scaling_fac = 0.08f;

    dest->x = 0;
    dest->y = top_navigation_bar->h + gap;
    dest->h = (float) out->h - dest->y - gap;
    dest->w = min_w + (float) out->w * w_scaling_fac;
    if (dest->w > max_w) {
        dest->w = max_w;
    }
}

static void calculate_canvas_boundaries(const SDL_Rect *const out,
                                        const SDL_FRect *const left_navigation_bar,
                                        const float gap,
                                        SDL_FRect *const dest) {
    dest->x = left_navigation_bar->x + left_navigation_bar->w + gap;
    dest->y = left_navigation_bar->y;
    dest->w = (float) out->w - dest->x - gap;
    dest->h = (float) out->h - dest->y - gap;
}

static void calculate_picked_color_boundaries(const SDL_FRect *const left_navigation_bar, SDL_FRect *const dest) {
    const float h_scaling_fac = 0.2f;

    dest->x = left_navigation_bar->x;
    dest->y = left_navigation_bar->y;
    dest->w = left_navigation_bar->w;
    dest->h = left_navigation_bar->h * h_scaling_fac;
}

static void calculate_palette_boundaries(const SDL_FRect *const left_navigation_bar,
                                         const SDL_FRect *const picked_color_rect,
                                         const float gap,
                                         SDL_FRect *dest) {
    dest->x = left_navigation_bar->x;
    dest->y = picked_color_rect->y + picked_color_rect->h + gap;
    dest->w = left_navigation_bar->w;
    dest->h = left_navigation_bar->h - picked_color_rect->h - gap;
}

static void render_color_palette(SDL_Renderer *const renderer,
                                 const SDL_FRect *const boundaries,
                                 const int colors_length,
                                 const int colors_per_row,
                                 const SDL_Color *const colors,
                                 SDL_FRect *const rects) {
    const float size = boundaries->w / (float) colors_per_row;

    SDL_FRect palette_box;
    palette_box.x = boundaries->x;
    palette_box.y = boundaries->y;
    palette_box.w = size;
    palette_box.h = size;

    for (int i = 0; i < colors_length; ++i) {
        SDL_FRect *box = &rects[i];
        const SDL_Color color = colors[i];

        SDL_Point fac;
        fac.x = i % colors_per_row;
        fac.y = i / colors_per_row;

        *box = palette_box;
        box->x += box->w * (float) fac.x;
        box->y += box->h * (float) fac.y;

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, box);
    }
}

static void display_frames_per_second(SDL_Window *const window, const uint64_t start) {
    const float elapsed = (float) (SDL_GetPerformanceCounter() - start) /
                          (float) SDL_GetPerformanceFrequency() * 1000.0f;
    const float fps = 1000.0f / elapsed;

    char *title;
    SDL_asprintf(&title, "Pixel Draw | FPS: %d", (int) fps);
    SDL_SetWindowTitle(window, title);
    free(title);
}

static uint32_t convert_rgba_to_hex(const SDL_Color *const color) {
    return color->r << 24 |
           color->g << 16 |
           color->b << 8 |
           color->a;
}

static void iterate(SDL_Renderer *renderer) {
    SDL_Window *window = SDL_GetRenderWindow(renderer);
    SDL_Event event;
    int is_running = true;

    const SDL_Color palette[] = {red, green, blue, yellow, magenta, cyan, white, black};
    const int palette_length = sizeof(palette) / sizeof(palette[0]);
    SDL_FRect palette_color_rects[palette_length];

    SDL_Rect out;
    SDL_FRect top_navigation_bar;
    SDL_FRect left_navigation_bar;
    SDL_FRect canvas_rect;
    SDL_FRect picked_color_rect;
    SDL_FRect color_palette_rect;

    SDL_FPoint mouse_pos;
    SDL_MouseButtonFlags mouse_button_flags;

    SDL_Color picked_color = white;
    uint32_t picked_color_hex = convert_rgba_to_hex(&picked_color);

    SDL_Texture *canvas = SDL_CreateTexture(renderer,
                                            SDL_PIXELFORMAT_RGBA8888,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            CANVAS_MAX_WIDTH,
                                            CANVAS_MAX_HEIGHT);
    float canvas_square_size = 0;

    SDL_SetTextureScaleMode(canvas, SDL_SCALEMODE_NEAREST);

    while (is_running) {
        const float gap = 4.0f;
        const int palette_colors_per_row = 2;
        const int canvas_w_px = CANVAS_MAX_WIDTH;
        const float gray_color_fac = 0.2f;
        const Uint64 now = SDL_GetPerformanceCounter();

        calculate_render_output_boundaries(renderer, &out);
        calculate_top_navigation_bar_boundaries(&out, &top_navigation_bar);
        calculate_left_navigation_bar_boundaries(&out, &top_navigation_bar, gap, &left_navigation_bar);
        calculate_canvas_boundaries(&out, &left_navigation_bar, gap, &canvas_rect);
        calculate_picked_color_boundaries(&left_navigation_bar, &picked_color_rect);
        calculate_palette_boundaries(&left_navigation_bar, &picked_color_rect, gap, &color_palette_rect);

        while (SDL_PollEvent(&event)) {
            mouse_button_flags = SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);

            switch (event.type) {
                case SDL_EVENT_QUIT:
                    is_running = false;
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (mouse_button_flags != SDL_BUTTON_LEFT) {
                        break;
                    }

                    if (is_point_intersects_rect(&mouse_pos, &color_palette_rect)) {
                        for (int i = 0; i < palette_length; ++i) {
                            if (is_point_intersects_rect(&mouse_pos, &palette_color_rects[i])) {
                                picked_color = palette[i];
                                picked_color_hex = convert_rgba_to_hex(&picked_color);
                                break;
                            }
                        }
                    }
                default:
                    break;
            }
        }

        // TODO: Enable mutation by loading from a file
        canvas_square_size = canvas_rect.w / (float) canvas_w_px;
        mouse_button_flags = SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);

        if (mouse_button_flags & SDL_BUTTON_LEFT && is_point_intersects_rect(&mouse_pos, &canvas_rect)) {
            const SDL_FPoint relative_pos = {.x = mouse_pos.x - canvas_rect.x, .y = mouse_pos.y - canvas_rect.y};
            const int col = (int) (relative_pos.x / canvas_square_size);
            const int row = (int) (relative_pos.y / canvas_square_size);
            const int index = col + row * canvas_w_px;

            uint32_t *pixels;
            int pitch;
            SDL_LockTexture(canvas, NULL, (void **) &pixels, &pitch);

            pixels[index] = picked_color_hex;

            SDL_UpdateTexture(canvas, NULL, pixels, pitch);
            SDL_UnlockTexture(canvas);
        }

        SDL_SetRenderDrawColor(renderer,
                               (int) ((float) white.r * gray_color_fac),
                               (int) ((float) white.g * gray_color_fac),
                               (int) ((float) white.b * gray_color_fac),
                               white.a);
        SDL_RenderClear(renderer);

        render_color_palette(renderer,
                             &color_palette_rect,
                             palette_length,
                             palette_colors_per_row,
                             palette,
                             palette_color_rects);

        SDL_SetRenderDrawColor(renderer, red.r, red.g, red.b, red.a);
        SDL_RenderRect(renderer, &top_navigation_bar);

        SDL_SetRenderDrawColor(renderer, picked_color.r, picked_color.g, picked_color.b, picked_color.a);
        SDL_RenderFillRect(renderer, &picked_color_rect);

        SDL_SetRenderDrawColor(renderer, green.r, green.g, green.b, green.a);
        SDL_RenderRect(renderer, &left_navigation_bar);

        SDL_FRect canvas_texture_rect = canvas_rect;
        canvas_texture_rect.h = canvas_texture_rect.w;
        SDL_RenderTexture(renderer, canvas, NULL, &canvas_texture_rect);

        SDL_SetRenderDrawColor(renderer, blue.r, blue.g, blue.b, blue.a);
        SDL_RenderRect(renderer, &canvas_rect);

        SDL_RenderPresent(renderer);

        display_frames_per_second(window, now);
    }

    SDL_DestroyTexture(canvas);
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
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    status = init();
    if (status != SDL_APP_CONTINUE) {
        return status;
    }

    status = create_window_and_renderer(&window, &renderer);
    if (status != SDL_APP_CONTINUE) {
        return status;
    }

    iterate(renderer);

    destroy_window_and_renderer(window, renderer);
    quit();
}
