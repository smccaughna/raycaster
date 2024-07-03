#include "global.h"
#include "render.h"
#include "texture.h"

#define FOV (M_PI / 2.0)
#define NUM_SECTORS 192

typedef struct ray
{
    SDL_FPoint point;
    SDL_Point block;
    enum {
        NORTH,
        SOUTH,
        EAST,
        WEST
    } side;
} ray_t;

static void draw_rect(uint32_t* pixels, SDL_Rect* rect, uint32_t color);
static void render_line(state_t* state, int32_t line);
static ray_t raycast(SDL_FPoint origin, float angle, map_t* map);

void render(state_t* state)
{
    draw_rect(state->surface->pixels, &(SDL_Rect){ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT / 2 }, 0xFF808080);
    draw_rect(state->surface->pixels, &(SDL_Rect){ 0, WINDOW_HEIGHT / 2, WINDOW_WIDTH, WINDOW_HEIGHT / 2 }, 0xFF000000);

    for (int i = 0; i < NUM_SECTORS; i++) {
        render_line(state, i);
    }

    char fps[5];
    itoa(state->time.fps, fps, 10);
    SDL_Surface* fps_text = TTF_RenderText_Blended(state->font, fps, (SDL_Color){ 0xFF, 0xFF, 0xFF, 0xFF });

    int32_t w, h;
    TTF_SizeText(state->font, fps, &w, &h);
    SDL_BlitSurface(fps_text, NULL, state->surface, &(SDL_Rect){ 10, 10, w, h });

    SDL_UpdateWindowSurface(state->window);
}

static void render_line(state_t* state, int32_t line)
{
    float plane_width = tanf(FOV / 2.0f) * 2;

    float angle = state->player.angle + atanf((line * (plane_width / NUM_SECTORS)) - (plane_width / 2.0f));
    ray_t ray = raycast(state->player.position, angle, &state->map);
    float distance = sqrtf(powf(state->player.position.x - ray.point.x, 2) + powf(state->player.position.y - ray.point.y, 2));
    int32_t sector_height = (1.0f / (distance * cosf(angle - state->player.angle)) * WINDOW_HEIGHT);

    float wall_x = (ray.side == NORTH || ray.side == SOUTH)
                   ? fmodf(ray.point.x, ray.block.x)
                   : fmodf(ray.point.y, ray.block.y);

    int32_t tex_x = (int32_t)(wall_x * TEXTURE_SIZE);
    if (ray.side == NORTH || ray.side == EAST) {
        tex_x = TEXTURE_SIZE - tex_x - 1;
    }

    uint32_t* tex_pixels = (uint32_t*)textures[state->map.grid[ray.block.y * state->map.width + ray.block.x] - 1]->pixels;

    float step = TEXTURE_SIZE / (float)sector_height;
    float tex_pos = (((WINDOW_HEIGHT - sector_height) / 2) - (WINDOW_HEIGHT / 2) + (sector_height / 2)) * step;

    for (int32_t i = (WINDOW_HEIGHT - sector_height) / 2; i < (WINDOW_HEIGHT + sector_height) / 2; i++) {
        int32_t tex_y = (int32_t)tex_pos & (TEXTURE_SIZE - 1);
        tex_pos += step;

        if (i > WINDOW_HEIGHT - 1 || i < 0) continue;

        uint32_t color = tex_pixels[tex_y * TEXTURE_SIZE + tex_x];

        if (ray.side == EAST || ray.side == WEST)
            color = 0xFF000000 | ((uint32_t)((color >> 16 & 0xFF) * 0.75f) << 16)
                               | ((uint32_t)((color >> 8  & 0xFF) * 0.75f) << 8)
                               | ((uint32_t)((color       & 0xFF) * 0.75f));

        color = 0xFF000000 | ((uint32_t)((color >> 16 & 0xFF) * (1.0f / (distance * 0.3f + 1))) << 16)
                           | ((uint32_t)((color >> 8  & 0xFF) * (1.0f / (distance * 0.3f + 1))) << 8)
                           | ((uint32_t)((color       & 0xFF) * (1.0f / (distance * 0.3f + 1))));

        draw_rect(state->surface->pixels, &(SDL_Rect){ line * (WINDOW_WIDTH / NUM_SECTORS), i, WINDOW_WIDTH / NUM_SECTORS, 1 }, color);
    }
}

static ray_t raycast(SDL_FPoint origin, float angle, map_t* map)
{
    SDL_FPoint ray = { cosf(angle), sinf(angle) };
    SDL_FPoint step = { sqrtf(1 + powf(ray.y / ray.x, 2)), sqrtf(1 + powf(ray.x / ray.y, 2)) };
    SDL_Point cell = { (int32_t)origin.x, (int32_t)origin.y };

    SDL_FPoint length;
    SDL_Point direction;
    int32_t side;

    if (ray.x < 0) {
        direction.x = -1;
        length.x = (origin.x - cell.x) * step.x;
    }
    else {
        direction.x = 1;
        length.x = (1 - (origin.x - cell.x)) * step.x;
    }

    if (ray.y < 0) {
        direction.y = -1;
        length.y = (origin.y - cell.y) * step.y;
    }
    else {
        direction.y = 1;
        length.y = (1 - (origin.y - cell.y)) * step.y;
    }

    float magnitude = 0.0f;
    while (magnitude < 1000.0f) {
        if (length.x > length.y) {
            cell.y += direction.y;
            magnitude = length.y;
            length.y += step.y;
            side = (direction.y > 0) ? NORTH : SOUTH;
        }
        else {
            cell.x += direction.x;
            magnitude = length.x;
            length.x += step.x;
            side = (direction.x > 0) ? WEST : EAST;
        }

        if (map->grid[cell.y * map->width + cell.x]) {
            return (ray_t){{ (origin.x + (ray.x * magnitude)), (origin.y + (ray.y * magnitude)) }, cell, side };
        }
    }

    return (ray_t){{ 0.0f, 0.0f }, { 0, 0 }, -1};
}

static void draw_rect(uint32_t* pixels, SDL_Rect* rect, uint32_t color)
{
    for (int32_t j = rect->y; j < rect->y + rect->h; j++) {
        for (int32_t i = rect->x; i < rect->x + rect->w; i++) {
            memcpy(pixels + (j * WINDOW_WIDTH) + i, &color, sizeof(uint32_t));
        }
    }
}