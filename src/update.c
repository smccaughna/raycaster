#include "global.h"
#include "update.h"

static bool check_map_collision(map_t* map, rectf* rectangle)
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

void update(state_t* state)
{
    if (state->keyboard[SDL_SCANCODE_W]) {
        vec2f new_pos = {
                state->player.position.x + state->player.speed * cosf(state->player.angle) * state->time.delta,
                state->player.position.y + state->player.speed * sinf(state->player.angle) * state->time.delta
        };

        if (!check_map_collision(&state->map, &(rectf){ new_pos.x - 0.1f, new_pos.y - 0.1f, 0.2f, 0.2f })) {
            state->player.position = new_pos;
        }
    }
    if (state->keyboard[SDL_SCANCODE_S]) {
        vec2f new_pos = {
                state->player.position.x - state->player.speed * cosf(state->player.angle) * state->time.delta,
                state->player.position.y - state->player.speed * sinf(state->player.angle) * state->time.delta
        };

        if (!check_map_collision(&state->map, &(rectf){ new_pos.x - 0.1f, new_pos.y - 0.1f, 0.2f, 0.2f })) {
            state->player.position = new_pos;
        }
    }
    if (state->keyboard[SDL_SCANCODE_A]) {
        vec2f new_pos = {
                state->player.position.x + state->player.speed * sinf(state->player.angle) * state->time.delta,
                state->player.position.y - state->player.speed * cosf(state->player.angle) * state->time.delta
        };

        if (!check_map_collision(&state->map, &(rectf){ new_pos.x - 0.1f, new_pos.y - 0.1f, 0.2f, 0.2f })) {
            state->player.position = new_pos;
        }
    }
    if (state->keyboard[SDL_SCANCODE_D]) {
        vec2f new_pos = {
                state->player.position.x - state->player.speed * sinf(state->player.angle) * state->time.delta,
                state->player.position.y + state->player.speed * cosf(state->player.angle) * state->time.delta
        };

        if (!check_map_collision(&state->map, &(rectf){ new_pos.x - 0.1f, new_pos.y - 0.1f, 0.2f, 0.2f })) {
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

uint32_t calculate_fps(uint32_t interval, void* param)
{
    ((state_t*)param)->time.fps = (int32_t)((float)(((state_t*)param)->time.tick - ((state_t*)param)->time.last) / ((float)interval / 1000.0f));
    ((state_t*)param)->time.last = ((state_t*)param)->time.tick;

    return 1000;
}