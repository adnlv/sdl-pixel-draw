#include "core/screen.h"
#include "core/canvas.h"
#include "core/storage.h"
#include "core/binary_storage.h"
#include "core/mouse.h"

#include <SDL3_image/SDL_image.h>

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

    const SDL_Rect relative_position = {
        .x = (int)mouse->pos.x - canvas->dimensions.x,
        .y = (int)mouse->pos.y - canvas->dimensions.y,
    };

    SDL_Point grid_position = {
        .x = relative_position.x / canvas->grid_cell_size.x,
        .y = relative_position.y / canvas->grid_cell_size.y,
    };

    /*
     * Make sure grid position doesn't go out of bounce.
     * Maybe I should've fixed the calculation algorithm in some way, but I am too lazy to do this.
     */
    grid_position.x = grid_position.x != canvas->grid_dimensions.w ? grid_position.x : grid_position.x - 1;
    grid_position.y = grid_position.y != canvas->grid_dimensions.h ? grid_position.y : grid_position.y - 1;

    const int grid_index = grid_position.y * canvas->grid_dimensions.w + grid_position.x;

    uint32_t* pixels;
    int pitch;
    SDL_LockTexture(canvas->texture, NULL, (void**)&pixels, &pitch);

    pixels[grid_index] = color_hex;

    SDL_UpdateTexture(canvas->texture, NULL, pixels, pitch);
    SDL_UnlockTexture(canvas->texture);
}

static bool save_texture(const storage_t* storage, const char* path, SDL_Texture* texture)
{
    FILE* saved = storage->open_file_stream(path);

    uint32_t* pixels;
    int pitch;
    SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);

    if (!storage->save_pixels(saved, texture->w, texture->h, pixels))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to save pixels");
        return false;
    }

    SDL_UnlockTexture(texture);
    storage->close_file_stream(saved);
    return true;
}

static bool load_canvas(const storage_t* storage, const char* path, canvas_t* canvas)
{
    FILE* saved = storage->open_file_stream(path);
    uint8_t saved_w = 0;
    uint8_t saved_h = 0;
    uint32_t saved_pixels[CANVAS_MAX_WIDTH * CANVAS_MAX_HEIGHT];

    if (!storage->read_pixels(saved, &saved_w, &saved_h, saved_pixels))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to save pixels");
        return false;
    }

    storage->close_file_stream(saved);

    uint32_t* pixels;
    int pitch;
    SDL_LockTexture(canvas->texture, NULL, (void**)&pixels, &pitch);

    for (int i = 0; i < saved_w * saved_h; ++i)
        pixels[i] = saved_pixels[i];

    SDL_UnlockTexture(canvas->texture);

    canvas->grid_dimensions.w = saved_w;
    canvas->grid_dimensions.w = saved_h;
    return true;
}

static void clear_texture(SDL_Texture* texture)
{
    uint32_t* pixels;
    int pitch;
    SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);

    for (int i = 0; i < texture->w * texture->h; ++i)
        pixels[i] = 0;

    SDL_UnlockTexture(texture);
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

    /* Temporary storage */
    storage.stream = storage.open_file_stream("tmp.bin");

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
        .num_cells = 9, /* Matches the amount of available colors */
        .cells_per_row = 3
    };

    struct
    {
        SDL_Texture* texture;
        uint32_t color_hex;
    } transparent = {
        .color_hex = 0,
        .texture = IMG_LoadTexture(renderer, "assets/forbidden.png")
    };
    if (transparent.texture == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load texture: %s", SDL_GetError());
        return;
    }

    /* Should be used only via `palette.cell_colors` and `palette.cell_colors` respectively */
    SDL_Color _palette_colors[] = {red, green, blue, yellow, cyan, magenta, white, black, (SDL_Color){0}};
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

            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_S)
                    save_texture(&storage, "saved.bin", canvas.texture);
                else if (event.key.key == SDLK_O)
                    load_canvas(&storage, "saved.bin", &canvas);
                else if (event.key.key == SDLK_C)
                    clear_texture(canvas.texture);
                else if (event.key.key == SDLK_F)
                    fill_texture_with_color(canvas.texture, picked_color.hex);

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
        if (picked_color.hex != transparent.color_hex)
        {
            render_color = picked_color.color;
            SDL_SetRenderDrawColor(screen.renderer, render_color.r, render_color.g, render_color.b, render_color.a);
            SDL_RenderFillRect(screen.renderer, &render_rect);
        }
        else
        {
            SDL_RenderTexture(renderer, transparent.texture, NULL, &render_rect);
        }

        render_color = picked_color.outline_color;
        SDL_SetRenderDrawColor(screen.renderer, render_color.r, render_color.g, render_color.b, render_color.a);
        SDL_RenderRect(screen.renderer, &render_rect);

        for (int i = 0; i < palette.num_cells - 1; ++i)
        {
            convert_rect_to_frect(&palette.cell_rects[i], &render_rect);
            render_color = palette.cell_colors[i];
            SDL_SetRenderDrawColor(screen.renderer, render_color.r, render_color.g, render_color.b, render_color.a);
            SDL_RenderFillRect(screen.renderer, &render_rect);
        }

        convert_rect_to_frect(&palette.cell_rects[palette.num_cells - 1], &render_rect);
        SDL_RenderTexture(renderer, transparent.texture, NULL, &render_rect);

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

    SDL_DestroyTexture(transparent.texture);

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

    // TODO: Destroy the window and renderer here instead of in run().
    destroy_sdl();
}
