#include "global.h"
#include "render.h"

#define NUM_SECTORS WINDOW_WIDTH

static SDL_FPoint raycast(SDL_FPoint origin, float angle, map_t* map);

void render(state_t* state)
{
    SDL_SetRenderDrawColor(state->renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(state->renderer);

    SDL_SetRenderDrawColor(state->renderer, 0x87, 0xCE, 0xEB, 0xFF);
    SDL_RenderFillRect(state->renderer, &(SDL_Rect){ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT / 2 });

    for (int i = 0; i < NUM_SECTORS; i++) {
        float angle = (state->player.angle - M_PI_4) + (i * (M_PI / (NUM_SECTORS * 2.0f)));
        SDL_FPoint ray = raycast(state->player.position, angle, &state->map);
        float distance = sqrtf(powf(state->player.position.x - ray.x, 2) + powf(state->player.position.y - ray.y, 2));
        int32_t sector_height = (1.0f / (distance * cosf(angle - state->player.angle) + 1)) * WINDOW_HEIGHT;

        SDL_SetRenderDrawColor(state->renderer, 0xFF - (distance * 10), 0xFF - (distance * 10), 0xFF - (distance * 10), 0xFF);
        SDL_RenderFillRect(state->renderer, &(SDL_Rect){ i * (WINDOW_WIDTH / NUM_SECTORS), (WINDOW_HEIGHT - sector_height) / 2, WINDOW_WIDTH / NUM_SECTORS, sector_height });
    }

    // render_map(state, (SDL_Point){ 0, 0 }, 80);

    SDL_RenderPresent(state->renderer);
}

void render_map(state_t* state, SDL_Point position, int32_t scale)
{
    SDL_SetRenderDrawColor(state->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(state->renderer, &(SDL_Rect){ position.x, position.y, state->map.width * scale, state->map.width * scale });

    SDL_SetRenderDrawColor(state->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    for (int i = position.x; i < state->map.width; i++) {
        for (int j = position.y; j < state->map.height; j++) {
            if (state->map.grid[j * state->map.width + i]) {
                SDL_RenderFillRect(state->renderer, &(SDL_Rect){ i * scale, j * scale, scale, scale });
            }
        }
    }

    SDL_SetRenderDrawColor(state->renderer, 0x00, 0x00, 0xFF, 0x00);
    SDL_RenderFillRect(state->renderer, &(SDL_Rect){ (int32_t)((state->player.position.x) * scale) - (scale / 10), (int32_t)(state->player.position.y * scale) - (scale / 10), scale / 5, scale / 5 });

    SDL_SetRenderDrawColor(state->renderer, 0x00, 0xFF, 0x00, 0xFF);
    for (int i = 0; i < NUM_SECTORS; i++) {
        SDL_FPoint ray = raycast(state->player.position, (state->player.angle - M_PI_4) + (i * (M_PI / (NUM_SECTORS * 2.0f))), &state->map);
        SDL_RenderDrawLine(state->renderer, state->player.position.x * scale, state->player.position.y * scale, ray.x * scale, ray.y * scale);
    }
}

static SDL_FPoint raycast(SDL_FPoint origin, float angle, map_t* map)
{
    SDL_FPoint ray = { cosf(angle), sinf(angle) };
    SDL_FPoint step = { sqrtf(1 + powf(ray.y / ray.x, 2)), sqrtf(1 + powf(ray.x / ray.y, 2)) };
    SDL_Point cell = {(int32_t)origin.x, (int32_t)origin.y };

    SDL_FPoint length;
    SDL_Point direction;

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
        }
        else {
            cell.x += direction.x;
            magnitude = length.x;
            length.x += step.x;
        }

        if (map->grid[cell.y * map->width + cell.x]) {
            return (SDL_FPoint){ (origin.x + (ray.x * magnitude)), (origin.y + (ray.y * magnitude)) };
        }
    }

    return (SDL_FPoint){ 0.0f, 0.0f };
}