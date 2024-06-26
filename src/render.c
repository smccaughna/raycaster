#include "global.h"
#include "render.h"

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

static ray_t raycast(SDL_FPoint origin, float angle, map_t* map);

void render(state_t* state)
{
    SDL_FillRect(state->surface, &(SDL_Rect){ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }, 0x000000);
    SDL_FillRect(state->surface, &(SDL_Rect){ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT / 2 }, SDL_MapRGB(state->surface->format, 0x87, 0xCE, 0xEB));

    float plane_width = tanf(FOV / 2.0f) * 2;
    for (int i = 0; i < NUM_SECTORS; i++) {
        float angle = state->player.angle + atanf((i * (plane_width / NUM_SECTORS)) - (plane_width / 2.0f));
        ray_t ray = raycast(state->player.position, angle, &state->map);
        float distance = sqrtf(powf(state->player.position.x - ray.point.x, 2) + powf(state->player.position.y - ray.point.y, 2));
        int32_t sector_height = (1.0f / (distance * cosf(angle - state->player.angle)) * WINDOW_HEIGHT);

        SDL_Color color;
        switch (state->map.grid[ray.block.y * state->map.width + ray.block.x]) {
            case 2:
                color = (SDL_Color){ 0xFF - (distance * 10), 0x00, 0x00, 0xFF };
                break;
            case 3:
                color = (SDL_Color){ 0x00, 0xFF - (distance * 10), 0x00, 0xFF };
                break;
            case 4:
                color = (SDL_Color){ 0x00, 0x00, 0xFF - (distance * 10), 0xFF };
                break;
            default:
                color = (SDL_Color){ 0xFF - (distance * 10), 0xFF - (distance * 10), 0xFF - (distance * 10), 0xFF };
        }

        if (ray.side == NORTH || ray.side == WEST) {
            if (color.r > 0) color.r -= 0x60;
            if (color.g > 0) color.g -= 0x60;
            if (color.b > 0) color.b -= 0x60;
        }

        if (color.r < 0) color.r = 0;
        if (color.g < 0) color.g = 0;
        if (color.b < 0) color.b = 0;

        SDL_FillRect(state->surface, &(SDL_Rect){ i * (WINDOW_WIDTH / NUM_SECTORS), (WINDOW_HEIGHT - sector_height) / 2, WINDOW_WIDTH / NUM_SECTORS, sector_height }, SDL_MapRGB(state->surface->format, color.r, color.g, color.b));
    }

    SDL_UpdateWindowSurface(state->window);
}

static ray_t raycast(SDL_FPoint origin, float angle, map_t* map)
{
    SDL_FPoint ray = { cosf(angle), sinf(angle) };
    SDL_FPoint step = { sqrtf(1 + powf(ray.y / ray.x, 2)), sqrtf(1 + powf(ray.x / ray.y, 2)) };
    SDL_Point cell = {(int32_t)origin.x, (int32_t)origin.y };

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
            side = (direction.y > 0) ? SOUTH : NORTH;
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