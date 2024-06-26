#include "global.h"
#include "update.h"

static bool check_map_collision(map_t* map, SDL_FRect* rectangle);

void update(state_t* state)
{
    if (state->keyboard[SDL_SCANCODE_W]) {
        SDL_FPoint new_pos = {
                state->player.position.x + state->player.speed * cosf(state->player.angle) * state->time.delta,
                state->player.position.y + state->player.speed * sinf(state->player.angle) * state->time.delta
        };

        if (!check_map_collision(&state->map, &(SDL_FRect){ new_pos.x - 0.1f, new_pos.y - 0.1f, 0.2f, 0.2f })) {
            state->player.position = new_pos;
        }
    }
    if (state->keyboard[SDL_SCANCODE_S]) {
        SDL_FPoint new_pos = {
                state->player.position.x - state->player.speed * cosf(state->player.angle) * state->time.delta,
                state->player.position.y - state->player.speed * sinf(state->player.angle) * state->time.delta
        };

        if (!check_map_collision(&state->map, &(SDL_FRect){ new_pos.x - 0.1f, new_pos.y - 0.1f, 0.2f, 0.2f })) {
            state->player.position = new_pos;
        }
    }
    if (state->keyboard[SDL_SCANCODE_A]) {
        SDL_FPoint new_pos = {
                state->player.position.x + state->player.speed * sinf(state->player.angle) * state->time.delta,
                state->player.position.y - state->player.speed * cosf(state->player.angle) * state->time.delta
        };

        if (!check_map_collision(&state->map, &(SDL_FRect){ new_pos.x - 0.1f, new_pos.y - 0.1f, 0.2f, 0.2f })) {
            state->player.position = new_pos;
        }
    }
    if (state->keyboard[SDL_SCANCODE_D]) {
        SDL_FPoint new_pos = {
                state->player.position.x - state->player.speed * sinf(state->player.angle) * state->time.delta,
                state->player.position.y + state->player.speed * cosf(state->player.angle) * state->time.delta
        };

        if (!check_map_collision(&state->map, &(SDL_FRect){ new_pos.x - 0.1f, new_pos.y - 0.1f, 0.2f, 0.2f })) {
            state->player.position = new_pos;
        }
    }

    if (state->keyboard[SDL_SCANCODE_LEFT]) {
        state->player.angle -= M_PI * state->time.delta;
        if (state->player.angle < 0) {
            state->player.angle += 2 * M_PI;
        }
    }
    if (state->keyboard[SDL_SCANCODE_RIGHT]) {
        state->player.angle += M_PI * state->time.delta;
        if (state->player.angle >= 2 * M_PI) {
            state->player.angle -= 2 * M_PI;
        }
    }
}

static bool check_map_collision(map_t* map, SDL_FRect* rectangle)
{
    for (int i = (int32_t)rectangle->x - 1; i <= (int32_t)rectangle->x + 1; i++) {
        for (int j = (int32_t)rectangle->y - 1; j <= (int32_t)rectangle->y + 1; j++) {
            if (map->grid[j * map->width + i] && check_collision(rectangle, &(SDL_FRect){ i, j, 1.0f, 1.0f })) {
                return true;
            }
        }
    }

    return false;
}