#include "core/screen.h"
#include "core/canvas.h"
#include "core/storage.h"
#include "core/binary_storage.h"
#include "core/mouse.h"

static const SDL_Color white = {0xFF, 0xFF, 0xFF, 0xFF};
static const SDL_Color black = {0, 0, 0, 0xFF};
static const SDL_Color gray = {0x20, 0x20, 0x20, 0xFF};
static const SDL_Color red = {0xFF, 0, 0, 0xFF};
static const SDL_Color green = {0, 0xFF, 0, 0xFF};
static const SDL_Color blue = {0, 0, 0xFF, 0xFF};
static const SDL_Color yellow = {0xFF, 0xFF, 0x00, 0xFF};
static const SDL_Color magenta = {0xFF, 0x00, 0xFF, 0xFF};
static const SDL_Color cyan = {0x00, 0xFF, 0xFF, 0xFF};

static int init_sdl(void)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

static void destroy_sdl(void)
{
    SDL_Quit();
}

static int create_window_and_renderer(SDL_Window** window, SDL_Renderer** renderer)
{
    const int width = 720;
    const int height = 480;
    const char title[] = "Pixel Draw";
    const SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_HIGH_PIXEL_DENSITY;

    if (!SDL_CreateWindowAndRenderer(title, width, height, flags, window, renderer))
    {
        SDL_Log("Failed to create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_SetRenderVSync(*renderer, 1))
    {
        SDL_Log("Failed to enable VSync: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

// static bool is_point_intersects_rect(const SDL_FPoint* const point, const SDL_FRect* const rect)
// {
//     return (int)point->x > (int)rect->x &&
//         (int)point->x < (int)(rect->x + rect->w) &&
//         (int)point->y > (int)rect->y &&
//         (int)point->y < (int)(rect->y + rect->h);
// }
//
// static void calculate_render_output_boundaries(SDL_Renderer* renderer, SDL_Rect* const dest)
// {
//     dest->x = 0;
//     dest->y = 0;
//
//     SDL_GetCurrentRenderOutputSize(renderer, &dest->w, &dest->h);
// }
//
// static void calculate_top_navigation_bar_boundaries(const SDL_Rect* const out, SDL_FRect* const dest)
// {
//     const float min_h = 40.0f;
//     const float max_h = 50.0f;
//     const float h_scaling_fac = 0.01f;
//
//     dest->x = 0;
//     dest->y = 0;
//     dest->w = (float)out->w;
//     dest->h = min_h + (float)out->h * h_scaling_fac;
//     if (dest->h > max_h)
//     {
//         dest->h = max_h;
//     }
// }
//
// static void calculate_left_navigation_bar_boundaries(const SDL_Rect* const out,
//                                                      const SDL_FRect* const top_navigation_bar,
//                                                      const float gap,
//                                                      SDL_FRect* const dest)
// {
//     const float min_w = 60.0f;
//     const float max_w = 160.0f;
//     const float w_scaling_fac = 0.08f;
//
//     dest->x = 0;
//     dest->y = top_navigation_bar->h + gap;
//     dest->h = (float)out->h - dest->y - gap;
//     dest->w = min_w + (float)out->w * w_scaling_fac;
//     if (dest->w > max_w)
//     {
//         dest->w = max_w;
//     }
// }
//
// static void calculate_canvas_boundaries(const SDL_Rect* const out,
//                                         const SDL_FRect* const left_navigation_bar,
//                                         const float gap,
//                                         SDL_FRect* const dest)
// {
//     dest->x = left_navigation_bar->x + left_navigation_bar->w + gap;
//     dest->y = left_navigation_bar->y;
//     dest->w = (float)out->w - dest->x - gap;
//     dest->h = (float)out->h - dest->y - gap;
// }
//
// static void calculate_picked_color_boundaries(const SDL_FRect* const left_navigation_bar, SDL_FRect* const dest)
// {
//     const float h_scaling_fac = 0.2f;
//
//     dest->x = left_navigation_bar->x;
//     dest->y = left_navigation_bar->y;
//     dest->w = left_navigation_bar->w;
//     dest->h = left_navigation_bar->h * h_scaling_fac;
// }
//
// static void calculate_palette_boundaries(const SDL_FRect* const left_navigation_bar,
//                                          const SDL_FRect* const picked_color_rect,
//                                          const float gap,
//                                          SDL_FRect* dest)
// {
//     dest->x = left_navigation_bar->x;
//     dest->y = picked_color_rect->y + picked_color_rect->h + gap;
//     dest->w = left_navigation_bar->w;
//     dest->h = left_navigation_bar->h - picked_color_rect->h - gap;
// }
//
// static void render_color_palette(SDL_Renderer* const renderer,
//                                  const SDL_FRect* const boundaries,
//                                  const int colors_length,
//                                  const int colors_per_row,
//                                  const SDL_Color* const colors,
//                                  SDL_FRect* const rects)
// {
//     const float size = boundaries->w / (float)colors_per_row;
//
//     SDL_FRect palette_box;
//     palette_box.x = boundaries->x;
//     palette_box.y = boundaries->y;
//     palette_box.w = size;
//     palette_box.h = size;
//
//     for (int i = 0; i < colors_length; ++i)
//     {
//         SDL_FRect* box = &rects[i];
//         const SDL_Color color = colors[i];
//
//         SDL_Point fac;
//         fac.x = i % colors_per_row;
//         fac.y = i / colors_per_row;
//
//         *box = palette_box;
//         box->x += box->w * (float)fac.x;
//         box->y += box->h * (float)fac.y;
//
//         SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
//         SDL_RenderFillRect(renderer, box);
//     }
// }
//
// static void display_frames_per_second(SDL_Window* const window, const uint64_t start)
// {
//     const float elapsed = (float)(SDL_GetPerformanceCounter() - start) /
//         (float)SDL_GetPerformanceFrequency() * 1000.0f;
//     const float fps = 1000.0f / elapsed;
//
//     char* title;
//     SDL_asprintf(&title, "Pixel Draw | FPS: %d", (int)fps);
//     SDL_SetWindowTitle(window, title);
//     free(title);
// }
//
// static uint32_t convert_rgba_to_hex(const SDL_Color* const color)
// {
//     return color->r << 24 |
//         color->g << 16 |
//         color->b << 8 |
//         color->a;
// }
//
// static void fill_canvas(SDL_Texture* const canvas, const uint32_t* pixels)
// {
//     uint32_t* old_pixels;
//     int pitch;
//     SDL_LockTexture(canvas, NULL, (void**)&old_pixels, &pitch);
//
//     for (int i = 0; i < canvas->w * canvas->h; ++i)
//     {
//         old_pixels[i] = pixels[i];
//     }
//
//     SDL_UnlockTexture(canvas);
// }
//
// static void fill_canvas_with_color(SDL_Texture* const canvas, const uint32_t color_hex)
// {
//     uint32_t* pixels;
//     int pitch;
//     SDL_LockTexture(canvas, NULL, (void**)&pixels, &pitch);
//
//     for (int i = 0; i < canvas->w * canvas->h; ++i)
//     {
//         pixels[i] = color_hex;
//     }
//
//     SDL_UnlockTexture(canvas);
// }
//
// static void recreate_canvas_texture(SDL_Renderer* const renderer,
//                                     SDL_Texture** const canvas,
//                                     const uint8_t w,
//                                     const uint8_t h)
// {
//     if (*canvas != NULL)
//     {
//         SDL_DestroyTexture(*canvas);
//     }
//
//     *canvas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h);
//     SDL_SetTextureScaleMode(*canvas, SDL_SCALEMODE_NEAREST);
// }
//
// static void reopen_binary_file(FILE** file)
// {
//     if (*file != NULL)
//         close_binary_file(*file);
//
//     *file = open_binary_file("image.bin");
// }
//
// static void run(state_t* state)
// {
//     SDL_Event event;
//     int is_running = true;
//
//     const SDL_Color palette[] = {red, green, blue, yellow, magenta, cyan, white, black};
//     const int palette_length = sizeof(palette) / sizeof(palette[0]);
//     SDL_FRect palette_color_rects[palette_length];
//
//     SDL_Rect out;
//     SDL_FRect top_navigation_bar;
//     SDL_FRect left_navigation_bar;
//     SDL_FRect picked_color_rect;
//     SDL_FRect color_palette_rect;
//
//     SDL_Color picked_color = white;
//     uint32_t picked_color_hex = convert_rgba_to_hex(&picked_color);
//
//     fill_canvas_with_color(state->canvas->texture, convert_rgba_to_hex(&black));
//
//     float canvas_square_size = 0;
//
//     while (is_running)
//     {
//         const float gap = 4.0f;
//         const int palette_colors_per_row = 2;
//         const int canvas_w_px = CANVAS_MAX_WIDTH;
//         const float gray_color_fac = 0.2f;
//         const Uint64 now = SDL_GetPerformanceCounter();
//
//         calculate_render_output_boundaries(state->screen->renderer, &out);
//         calculate_top_navigation_bar_boundaries(&out, &top_navigation_bar);
//         calculate_left_navigation_bar_boundaries(&out, &top_navigation_bar, gap, &left_navigation_bar);
//         calculate_canvas_boundaries(&out, &left_navigation_bar, gap, &state->canvas->dimensions);
//         calculate_picked_color_boundaries(&left_navigation_bar, &picked_color_rect);
//         calculate_palette_boundaries(&left_navigation_bar, &picked_color_rect, gap, &color_palette_rect);
//
//         while (SDL_PollEvent(&event))
//         {
//             switch (event.type)
//             {
//             case SDL_EVENT_QUIT:
//                 is_running = false;
//                 break;
//             case SDL_EVENT_KEY_DOWN:
//                 if (event.key.key == SDLK_S)
//                 {
//                     uint32_t* pixels;
//                     int pitch;
//                     SDL_LockTexture(state->canvas->texture, NULL, (void**)&pixels, &pitch);
//
//                     reopen_binary_file(&state->storage->stream);
//                     if (!state->storage->save_pixels(state->storage->stream,
//                                                      state->canvas->texture->w,
//                                                      state->canvas->texture->h,
//                                                      pixels))
//                     {
//                         SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to save pixels to binary");
//                         return;
//                     }
//                     close_binary_file(state->storage->stream);
//                     state->storage->stream = NULL;
//
//                     SDL_UnlockTexture(state->canvas->texture);
//                 }
//                 else if (event.key.key == SDLK_O)
//                 {
//                     uint8_t new_w = 0;
//                     uint8_t new_h = 0;
//                     uint32_t pixels[CANVAS_MAX_WIDTH * CANVAS_MAX_HEIGHT];
//
//                     reopen_binary_file(&state->storage->stream);
//                     if (!state->storage->read_pixels(state->storage->stream, &new_w, &new_h, pixels))
//                     {
//                         SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to read pixels from binary");
//                         return;
//                     }
//                     close_binary_file(state->storage->stream);
//                     state->storage->stream = NULL;
//
//                     recreate_canvas_texture(state->screen->renderer, &state->canvas->texture, new_w, new_h);
//                     fill_canvas(state->canvas->texture, pixels);
//
//                     SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Read %d pixels from binary", new_w * new_h);
//                 }
//             case SDL_EVENT_MOUSE_BUTTON_DOWN:
//                 update_mouse_state(&state->mouse);
//
//                 if (state->mouse.btn != SDL_BUTTON_LEFT)
//                 {
//                     break;
//                 }
//
//                 if (is_point_intersects_rect(&state->mouse.pos, &color_palette_rect))
//                 {
//                     for (int i = 0; i < palette_length; ++i)
//                     {
//                         if (is_point_intersects_rect(&state->mouse.pos, &palette_color_rects[i]))
//                         {
//                             picked_color = palette[i];
//                             picked_color_hex = convert_rgba_to_hex(&picked_color);
//                             break;
//                         }
//                     }
//                 }
//             default:
//                 break;
//             }
//         }
//
//         // TODO: Enable mutation by loading from a file
//         canvas_square_size = state->canvas->dimensions.w / (float)canvas_w_px;
//         update_mouse_state(&state->mouse);
//
//         if (state->mouse.btn & SDL_BUTTON_LEFT &&
//             is_point_intersects_rect(&state->mouse.pos, &state->canvas->dimensions))
//         {
//             const SDL_FPoint relative_pos = {
//                 .x = state->mouse.pos.x - state->canvas->dimensions.x,
//                 .y = state->mouse.pos.y - state->canvas->dimensions.y
//             };
//             const int col = (int)(relative_pos.x / canvas_square_size);
//             const int row = (int)(relative_pos.y / canvas_square_size);
//             const int index = col + row * canvas_w_px;
//
//             uint32_t* pixels;
//             int pitch;
//             SDL_LockTexture(state->canvas->texture, NULL, (void**)&pixels, &pitch);
//
//             pixels[index] = picked_color_hex;
//
//             SDL_UpdateTexture(state->canvas->texture, NULL, pixels, pitch);
//             SDL_UnlockTexture(state->canvas->texture);
//         }
//
//         SDL_SetRenderDrawColor(state->screen->renderer,
//                                (int)((float)white.r * gray_color_fac),
//                                (int)((float)white.g * gray_color_fac),
//                                (int)((float)white.b * gray_color_fac),
//                                white.a);
//         SDL_RenderClear(state->screen->renderer);
//
//         render_color_palette(state->screen->renderer,
//                              &color_palette_rect,
//                              palette_length,
//                              palette_colors_per_row,
//                              palette,
//                              palette_color_rects);
//
//         SDL_SetRenderDrawColor(state->screen->renderer, red.r, red.g, red.b, red.a);
//         SDL_RenderRect(state->screen->renderer, &top_navigation_bar);
//
//         SDL_SetRenderDrawColor(state->screen->renderer, picked_color.r, picked_color.g, picked_color.b, picked_color.a);
//         SDL_RenderFillRect(state->screen->renderer, &picked_color_rect);
//
//         SDL_SetRenderDrawColor(state->screen->renderer, green.r, green.g, green.b, green.a);
//         SDL_RenderRect(state->screen->renderer, &left_navigation_bar);
//
//         SDL_FRect canvas_texture_rect = state->canvas->dimensions;
//         canvas_texture_rect.h = canvas_texture_rect.w;
//         SDL_RenderTexture(state->screen->renderer, state->canvas->texture, NULL, &canvas_texture_rect);
//
//         SDL_SetRenderDrawColor(state->screen->renderer, blue.r, blue.g, blue.b, blue.a);
//         SDL_RenderRect(state->screen->renderer, &state->canvas->dimensions);
//
//         SDL_RenderPresent(state->screen->renderer);
//
//         display_frames_per_second(state->screen->window, now);
//     }
// }

static bool is_point_in_rect(const SDL_FPoint* point, const SDL_Rect* rect)
{
    return (int)point->x > rect->x &&
        (int)point->x < rect->x + rect->w &&
        (int)point->y > rect->y &&
        (int)point->y < rect->y + rect->h;
}

static void convert_rect_to_frect(const SDL_Rect* src, SDL_FRect* dest)
{
    dest->x = (float)src->x;
    dest->y = (float)src->y;
    dest->w = (float)src->w;
    dest->h = (float)src->h;
}

static uint32_t convert_rgba_to_hex(const SDL_Color* color)
{
    return color->r << 24 | color->g << 16 | color->b << 8 | color->a;
}

static void calculate_top_navigation_bar_dimensions(const int screen_w, const int screen_h, SDL_Rect* dest)
{
    const int min_top_navigation_bar_h = 40;
    const int max_top_navigation_bar_h = 60;
    const float top_navigation_bar_h_scale_fac = 0.025f;
    dest->w = screen_w;
    dest->h = min_top_navigation_bar_h +
        (int)((float)screen_h * top_navigation_bar_h_scale_fac);
    if (dest->h > max_top_navigation_bar_h)
        dest->h = max_top_navigation_bar_h;
}

static void calculate_left_navigation_bar_dimensions(const int screen_h,
                                                     const SDL_Rect* top_navigation_bar,
                                                     const int gap,
                                                     SDL_Rect* dest)
{
    const int min_left_navigation_bar_w = 60;
    const int max_left_navigation_bar_w = 160;
    const float left_navigation_bar_w_scale_fac = 0.1f;
    dest->x = top_navigation_bar->x;
    dest->y = top_navigation_bar->h + gap;
    dest->h = screen_h - dest->y;
    dest->w = min_left_navigation_bar_w +
        (int)((float)top_navigation_bar->w * left_navigation_bar_w_scale_fac);
    if (dest->w > max_left_navigation_bar_w)
        dest->w = max_left_navigation_bar_w;
}

static void calculate_canvas_dimensions(const SDL_Rect* top_navigation_bar,
                                        const SDL_Rect* left_navigation_bar,
                                        const int gap,
                                        SDL_Rect* dest)
{
    dest->x = left_navigation_bar->w + gap;
    dest->y = left_navigation_bar->y;
    dest->w = top_navigation_bar->w - dest->x;
    dest->h = left_navigation_bar->h;

    // Show the whole canvas no matter what
    if (dest->w > dest->h)
        dest->w = dest->h;
    else
        dest->h = dest->w;
}

static void calculate_picked_color_dimensions(const SDL_Rect* left_navigation_bar, SDL_Rect* dest)
{
    const int min_picked_color_h = 40;
    const int max_picked_color_h = 120;
    const float picked_color_h_scale_fac = 0.1f;
    dest->x = left_navigation_bar->x;
    dest->y = left_navigation_bar->y;
    dest->w = left_navigation_bar->w;
    dest->h = min_picked_color_h + (int)((float)left_navigation_bar->h * picked_color_h_scale_fac);
    if (dest->h > max_picked_color_h)
        dest->h = max_picked_color_h;
}

static void calculate_palette_dimensions(const int num_cells,
                                         const int cells_per_row,
                                         SDL_Rect* cell_rects,
                                         const SDL_Rect* picked_color,
                                         const int gap,
                                         SDL_Rect* dest)
{
    dest->x = picked_color->x;
    dest->y = picked_color->y + picked_color->h + gap;
    dest->w = picked_color->w;

    const int cell_size = dest->w / cells_per_row;
    for (int i = 0; i < num_cells; ++i)
    {
        const SDL_Point pos_index = {
            .x = i % cells_per_row,
            .y = i / cells_per_row
        };

        SDL_Rect* rect = &cell_rects[i];
        rect->x = dest->x + cell_size * pos_index.x;
        rect->y = dest->y + cell_size * pos_index.y;
        rect->w = cell_size;
        rect->h = cell_size;

        dest->h = rect->h * (pos_index.y + 1);
    }
}

static void fill_texture_with_color(SDL_Texture* texture, const uint32_t color_hex)
{
    uint32_t* pixels;
    int pitch;
    SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);
    for (int i = 0; i < texture->w * texture->h; ++i)
        pixels[i] = color_hex;

    SDL_UnlockTexture(texture);
}

static void draw_pixel(canvas_t* canvas, const mouse_state_t* mouse, const uint32_t color_hex)
{
    canvas->grid_cell_size.x = canvas->dimensions.w / canvas->grid_dimensions.w;
    canvas->grid_cell_size.y = canvas->grid_cell_size.x;

    const SDL_FRect relative_position = {
        .x = mouse->pos.x - (float)canvas->dimensions.x,
        .y = mouse->pos.y - (float)canvas->dimensions.y,
    };

    const SDL_Rect grid_position = {
        .x = (int)relative_position.x / canvas->grid_cell_size.x,
        .y = (int)relative_position.y / canvas->grid_cell_size.y,
    };

    const int grid_index = grid_position.y * canvas->grid_dimensions.w + grid_position.x;

    uint32_t* pixels;
    int pitch;
    SDL_LockTexture(canvas->texture, NULL, (void**)&pixels, &pitch);

    pixels[grid_index] = color_hex;

    SDL_UpdateTexture(canvas->texture, NULL, pixels, pitch);
    SDL_UnlockTexture(canvas->texture);
}

static void run(SDL_Window* window, SDL_Renderer* renderer)
{
    screen_t screen;
    canvas_t canvas;
    storage_t storage;
    mouse_state_t mouse;

    init_screen(window, renderer, &screen);
    init_canvas(renderer, CANVAS_DEFAULT_WIDTH, CANVAS_DEFAULT_WIDTH, &canvas);
    init_storage(&storage, open_binary_file, close_binary_file, save_pixels_to_binary, read_pixels_from_binary);

    const uint32_t canvas_default_color_hex = convert_rgba_to_hex(&black);
    fill_texture_with_color(canvas.texture, canvas_default_color_hex);

    struct
    {
        SDL_Rect rect;
        const SDL_Color outline_color;
    } top_navigation_bar = {
        .rect = (SDL_Rect){.x = 0, .y = 0},
        .outline_color = red
    };

    struct
    {
        SDL_Rect rect;
        const SDL_Color outline_color;
    } left_navigation_bar = {
        .rect = (SDL_Rect){.x = 0},
        .outline_color = green
    };

    struct
    {
        SDL_Rect rect;
        SDL_Color color;
        const SDL_Color outline_color;
        uint32_t hex;
    } picked_color = {
        .color = white,
        .hex = convert_rgba_to_hex(&white),
        .outline_color = cyan
    };

    struct
    {
        SDL_Rect rect;
        const SDL_Color outline_color;
        const int num_cells;
        const int cells_per_row;
        SDL_Color* cell_colors;
        SDL_Rect* cell_rects;
    } palette = {
        .outline_color = magenta,
        .num_cells = 8,
        .cells_per_row = 2
    };

    /* Should be used only via `palette.cell_colors` and `palette.cell_colors` respectfully */
    SDL_Color _palette_colors[] = {red, green, blue, yellow, cyan, magenta, white, black};
    SDL_Rect _palette_color_rects[palette.num_cells];

    palette.cell_colors = _palette_colors;
    palette.cell_rects = _palette_color_rects;

    SDL_Event event;
    bool is_running = true;
    while (is_running)
    {
        const int gap = 4;
        int screen_w;
        int screen_h;
        SDL_GetCurrentRenderOutputSize(screen.renderer, &screen_w, &screen_h);

        calculate_top_navigation_bar_dimensions(screen_w, screen_h, &top_navigation_bar.rect);
        calculate_left_navigation_bar_dimensions(screen_h, &top_navigation_bar.rect, gap, &left_navigation_bar.rect);
        calculate_canvas_dimensions(&top_navigation_bar.rect, &left_navigation_bar.rect, gap, &canvas.dimensions);
        calculate_picked_color_dimensions(&left_navigation_bar.rect, &picked_color.rect);
        calculate_palette_dimensions(palette.num_cells,
                                     palette.cells_per_row,
                                     palette.cell_rects,
                                     &picked_color.rect,
                                     gap,
                                     &palette.rect);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                is_running = false;
                break;
            }

            update_mouse_state(&mouse);
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                if (mouse.btn != SDL_BUTTON_LEFT)
                    break;

                if (is_point_in_rect(&mouse.pos, &palette.rect))
                {
                    for (int i = 0; i < palette.num_cells; ++i)
                    {
                        if (is_point_in_rect(&mouse.pos, &palette.cell_rects[i]))
                        {
                            picked_color.color = palette.cell_colors[i];
                            picked_color.hex = convert_rgba_to_hex(&picked_color.color);
                            break;
                        }
                    }
                }
            }
        }

        update_mouse_state(&mouse);
        if (mouse.btn == SDL_BUTTON_LEFT && is_point_in_rect(&mouse.pos, &canvas.dimensions))
            draw_pixel(&canvas, &mouse, picked_color.hex);

        SDL_FRect render_rect;
        SDL_Color render_color = gray;

        SDL_SetRenderDrawColor(screen.renderer, render_color.r, render_color.g, render_color.b, render_color.a);
        SDL_RenderClear(screen.renderer);

        convert_rect_to_frect(&top_navigation_bar.rect, &render_rect);
        render_color = top_navigation_bar.outline_color;
        SDL_SetRenderDrawColor(screen.renderer, render_color.r, render_color.g, render_color.b, render_color.a);
        SDL_RenderRect(screen.renderer, &render_rect);

        convert_rect_to_frect(&picked_color.rect, &render_rect);
        render_color = picked_color.color;
        SDL_SetRenderDrawColor(screen.renderer, render_color.r, render_color.g, render_color.b, render_color.a);
        SDL_RenderFillRect(screen.renderer, &render_rect);

        render_color = picked_color.outline_color;
        SDL_SetRenderDrawColor(screen.renderer, render_color.r, render_color.g, render_color.b, render_color.a);
        SDL_RenderRect(screen.renderer, &render_rect);

        for (int i = 0; i < palette.num_cells; ++i)
        {
            convert_rect_to_frect(&palette.cell_rects[i], &render_rect);
            render_color = palette.cell_colors[i];
            SDL_SetRenderDrawColor(screen.renderer, render_color.r, render_color.g, render_color.b, render_color.a);
            SDL_RenderFillRect(screen.renderer, &render_rect);
        }

        convert_rect_to_frect(&palette.rect, &render_rect);
        render_color = palette.outline_color;
        SDL_SetRenderDrawColor(screen.renderer, render_color.r, render_color.g, render_color.b, render_color.a);
        SDL_RenderRect(screen.renderer, &render_rect);

        convert_rect_to_frect(&left_navigation_bar.rect, &render_rect);
        render_color = left_navigation_bar.outline_color;
        SDL_SetRenderDrawColor(screen.renderer, render_color.r, render_color.g, render_color.b, render_color.a);
        SDL_RenderRect(screen.renderer, &render_rect);

        convert_rect_to_frect(&canvas.dimensions, &render_rect);
        SDL_RenderTexture(renderer, canvas.texture, NULL, &render_rect);

        convert_rect_to_frect(&canvas.dimensions, &render_rect);
        render_color = blue;
        SDL_SetRenderDrawColor(screen.renderer, render_color.r, render_color.g, render_color.b, render_color.a);
        SDL_RenderRect(screen.renderer, &render_rect);

        SDL_RenderPresent(screen.renderer);
    }

    destroy_canvas(&canvas);
    destroy_storage(&storage);
    destroy_screen(&screen);
}

int main(void)
{
    int status = init_sdl();
    if (status != SDL_APP_CONTINUE)
        return status;

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    status = create_window_and_renderer(&window, &renderer);
    if (status != SDL_APP_CONTINUE)
        return status;

    run(window, renderer);

    destroy_sdl();
}
