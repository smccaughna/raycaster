#include "global.h"
#include "render.h"
#include "texture.h"

#define FOV (M_PI / 2.0)
#define NUM_SECTORS (WINDOW_WIDTH)

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

static void render_line(state_t* state, int32_t line);
static ray_t raycast(SDL_FPoint origin, float angle, map_t* map);

void render(state_t* state)
{
    SDL_FillRect(state->surface, &(SDL_Rect){ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }, 0x000000);
    SDL_FillRect(state->surface, &(SDL_Rect){ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT / 2 }, SDL_MapRGB(state->surface->format, 0x87, 0xCE, 0xEB));

    for (int i = 0; i < NUM_SECTORS; i++) {
        render_line(state, i);
    }

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

    uint32_t* screen_pixels = (uint32_t*)state->surface->pixels;

    float step = TEXTURE_SIZE / (float)sector_height;
    float tex_pos = (((WINDOW_HEIGHT - sector_height) / 2) - (WINDOW_HEIGHT / 2) + (sector_height / 2)) * step;
    for (int32_t j = (WINDOW_HEIGHT - sector_height) / 2; j < (WINDOW_HEIGHT + sector_height) / 2; j++) {
        int32_t tex_y = (int32_t)tex_pos & (TEXTURE_SIZE - 1);
        tex_pos += step;

        if (j > WINDOW_HEIGHT - 1 || j < 0) continue;

        uint32_t color = tex_pixels[tex_y * TEXTURE_SIZE + tex_x];

        if (ray.side == EAST || ray.side == WEST)
            color = 0xFF000000 + ((uint32_t)((color >> 16 & 0xFF) * 0.75f) << 16) + ((uint32_t)((color >> 8 & 0xFF) * 0.75f) << 8) + (uint32_t)((color & 0xFF) * 0.75f);

        color = 0xFF000000 + ((uint32_t)((color >> 16 & 0xFF) * (1.0f / (distance * 0.2f + 1))) << 16) + ((uint32_t)((color >> 8 & 0xFF) * (1.0f / (distance * 0.2f + 1))) << 8) + (uint32_t)((color & 0xFF) * (1.0f / (distance * 0.2f + 1)));

        screen_pixels[j * WINDOW_WIDTH + line] = color;
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