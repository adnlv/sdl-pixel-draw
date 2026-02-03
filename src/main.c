#include <stdlib.h>
#include <SDL3/SDL.h>

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

    return SDL_APP_CONTINUE;
}

static bool is_point_intersects_rect(const SDL_FPoint *const point, const SDL_FRect *const rect) {
    return point->x >= rect->x &&
           point->x <= rect->x + rect->w &&
           point->y >= rect->y &&
           point->y <= rect->y + rect->h;
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

    int i = 0;
    for (; i < colors_length; ++i) {
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

static void iterate(SDL_Window *window, SDL_Renderer *renderer) {
    SDL_Event event;
    int is_running = true;
    const float gray_color_fac = 0.2f;

    const SDL_Color palette[] = {red, green, blue, yellow, magenta, cyan, white, black};
    const int palette_length = sizeof(palette) / sizeof(palette[0]);
    SDL_FRect palette_color_rects[palette_length];
    const int palette_colors_per_row = 2;

    const float gap = 4.0f;

    SDL_FRect top_nav;
    const float top_nav_min_h = 40.0f;
    const float top_nav_max_h = 50.0f;
    const float top_nav_h_scaling_fac = 0.01f;
    top_nav.x = 0;
    top_nav.y = 0;

    SDL_FRect left_nav;
    const float left_nav_min_w = 60.0f;
    const float left_nav_max_w = 160.0f;
    const float left_nav_w_scaling_fac = 0.08f;
    left_nav.x = 0;

    SDL_FRect picked_color_rect;
    SDL_FRect canvas;

    // TODO: Enable mutation via mouse events
    const SDL_Color picked_color = white;

    SDL_Rect out;
    out.x = 0;
    out.y = 0;

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

        SDL_GetCurrentRenderOutputSize(renderer, &out.w, &out.h);

        top_nav.w = (float) out.w;
        top_nav.h = top_nav_min_h + (float) out.h * top_nav_h_scaling_fac;
        if (top_nav.h > top_nav_max_h) {
            top_nav.h = top_nav_max_h;
        }

        left_nav.y = top_nav.h + gap;
        left_nav.h = (float) out.h - left_nav.y - gap;
        left_nav.w = left_nav_min_w + (float) out.w * left_nav_w_scaling_fac;
        if (left_nav.w > left_nav_max_w) {
            left_nav.w = left_nav_max_w;
        }

        canvas.x = left_nav.x + left_nav.w + gap;
        canvas.y = left_nav.y;
        canvas.w = (float) out.w - canvas.x - gap;
        canvas.h = (float) out.h - canvas.y - gap;

        picked_color_rect.x = left_nav.x;
        picked_color_rect.y = left_nav.y;
        picked_color_rect.w = left_nav.w;
        picked_color_rect.h = left_nav.h * 0.2f;

        SDL_SetRenderDrawColor(renderer,
                               (int) ((float) white.r * gray_color_fac),
                               (int) ((float) white.g * gray_color_fac),
                               (int) ((float) white.b * gray_color_fac),
                               white.a);
        SDL_RenderClear(renderer);

        SDL_FRect color_palette_rect;
        color_palette_rect.x = left_nav.x;
        color_palette_rect.y = picked_color_rect.y + picked_color_rect.h + gap;
        color_palette_rect.w = left_nav.w;
        color_palette_rect.h = left_nav.h - picked_color_rect.h - gap;

        render_color_palette(renderer,
                             &color_palette_rect,
                             palette_length,
                             palette_colors_per_row,
                             palette,
                             palette_color_rects);

        SDL_SetRenderDrawColor(renderer, red.r, red.g, red.b, red.a);
        SDL_RenderRect(renderer, &top_nav);
        SDL_SetRenderDrawColor(renderer, picked_color.r, picked_color.g, picked_color.b, picked_color.a);
        SDL_RenderFillRect(renderer, &picked_color_rect);
        SDL_SetRenderDrawColor(renderer, green.r, green.g, green.b, green.a);
        SDL_RenderRect(renderer, &left_nav);
        SDL_SetRenderDrawColor(renderer, blue.r, blue.g, blue.b, blue.a);
        SDL_RenderRect(renderer, &canvas);
        SDL_RenderPresent(renderer);
    }
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

    iterate(window, renderer);

    destroy_window_and_renderer(window, renderer);
    quit();
}
