#include "global.h"
#include "render.h"
#include "texture.h"

#define FOV M_PI_2
#define NUM_COLUMNS 192

static void draw_debug_info(state_t* state, bool show_advanced);
static ray_t raycast(vec2f origin, float angle, map_t* map);

static void draw_rect(uint32_t* pixels, rect* rect, uint32_t color)
{
    for (int32_t j = rect->y; j < rect->y + rect->h; j++) {
        for (int32_t i = rect->x; i < rect->x + rect->w; i++) {
            memcpy(pixels + (j * WINDOW_WIDTH) + i, &color, sizeof(uint32_t));
        }
    }
}

void render(state_t* state)
{
    const float plane_width = tanf(FOV / 2.0f) * 2.0f;
    const int32_t px_per_sector = WINDOW_WIDTH / NUM_COLUMNS;

    for (int column = 0; column < NUM_COLUMNS; column++) {
        float ray_angle = state->player.angle + atanf((column * (plane_width / NUM_COLUMNS)) - (plane_width / 2.0f));

        ray_t ray = raycast(state->player.position, ray_angle, &state->map);
        float distance = sqrtf(powf(state->player.position.x - ray.point.x, 2) + powf(state->player.position.y - ray.point.y, 2));

        float perp_distance = distance * cosf(ray_angle - state->player.angle);
        int32_t wall_height = (int32_t)(WINDOW_HEIGHT / perp_distance);

        float wall_x = (ray.side == NORTH || ray.side == SOUTH)
                       ? fmodf(ray.point.x, ray.cell.x)
                       : fmodf(ray.point.y, ray.cell.y);

        vec2i wall_tex;
        wall_tex.x = (int32_t)(wall_x * TEXTURE_SIZE);
        if (ray.side == NORTH || ray.side == EAST)
            wall_tex.x = TEXTURE_SIZE - wall_tex.x - 1;

        int32_t wall_start = (WINDOW_HEIGHT - wall_height) / 2;
        int32_t wall_end = (WINDOW_HEIGHT + wall_height) / 2;

        float step = TEXTURE_SIZE / (float)wall_height;
        float tex_pos = 0.0f;

        for (int32_t i = wall_start; i < wall_end; i++) {
            wall_tex.y = (int32_t)tex_pos;
            tex_pos += step;

            if (i > WINDOW_HEIGHT - 1 || i < 0) continue;

            uint32_t color = ((uint32_t*)state->textures[state->map.grid[ray.cell.y * state->map.width + ray.cell.x] - 1]->pixels)[wall_tex.y * TEXTURE_SIZE + wall_tex.x];

            if (ray.side == EAST || ray.side == WEST)
                color = 0xFF000000 | ((uint32_t)((color >> 16 & 0xFF) * 0.75f) << 16)
                                   | ((uint32_t)((color >> 8  & 0xFF) * 0.75f) << 8)
                                   | ((uint32_t)((color       & 0xFF) * 0.75f));

            color = 0xFF000000 | ((uint32_t)((color >> 16 & 0xFF) * (1.0f / (distance * 0.3f + 1))) << 16)
                               | ((uint32_t)((color >> 8  & 0xFF) * (1.0f / (distance * 0.3f + 1))) << 8)
                               | ((uint32_t)((color       & 0xFF) * (1.0f / (distance * 0.3f + 1))));

            draw_rect(state->surface->pixels, &(rect){ column * px_per_sector, i, px_per_sector, 1 }, color);
        }

        vec2f floor_pos = {0.0f };
        switch (ray.side) {
            case NORTH:
                floor_pos = (vec2f){ ray.point.x, ray.cell.y };
                break;
            case SOUTH:
                floor_pos = (vec2f){ ray.point.x, ray.cell.y + 1.0f };
                break;
            case EAST:
                floor_pos = (vec2f){ ray.cell.x + 1.0f, ray.point.y };
                break;
            case WEST:
                floor_pos = (vec2f){ ray.cell.x, ray.point.y };
                break;
        }

        for (int32_t i = wall_end; i < WINDOW_HEIGHT; i++) {
            float current_floor_distance = WINDOW_HEIGHT / (2.0f * i - WINDOW_HEIGHT);
            float weight = current_floor_distance / perp_distance;

            vec2f current_floor_pos = {
                    weight * floor_pos.x + (1.0f - weight) * state->player.position.x,
                    weight * floor_pos.y + (1.0f - weight) * state->player.position.y
            };

            vec2i floor_tex = {
                    (int32_t)(current_floor_pos.x * TEXTURE_SIZE) % TEXTURE_SIZE,
                    (int32_t)(current_floor_pos.y * TEXTURE_SIZE) % TEXTURE_SIZE
            };

            uint32_t floor_color = ((uint32_t*)state->textures[3]->pixels)[floor_tex.y * TEXTURE_SIZE + floor_tex.x];
            uint32_t ceiling_color = ((uint32_t*)state->textures[1]->pixels)[floor_tex.y * TEXTURE_SIZE + floor_tex.x];

            floor_color = 0xFF000000 | ((uint32_t)((floor_color >> 16 & 0xFF) * (1.0f / (current_floor_distance * 0.3f + 1))) << 16)
                                     | ((uint32_t)((floor_color >> 8  & 0xFF) * (1.0f / (current_floor_distance * 0.3f + 1))) << 8)
                                     | ((uint32_t)((floor_color       & 0xFF) * (1.0f / (current_floor_distance * 0.3f + 1))));

            ceiling_color = 0xFF000000 | ((uint32_t)((ceiling_color >> 16 & 0xFF) * (1.0f / (current_floor_distance * 0.3f + 1))) << 16)
                                       | ((uint32_t)((ceiling_color >> 8  & 0xFF) * (1.0f / (current_floor_distance * 0.3f + 1))) << 8)
                                       | ((uint32_t)((ceiling_color       & 0xFF) * (1.0f / (current_floor_distance * 0.3f + 1))));

            draw_rect(state->surface->pixels, &(rect){ column * px_per_sector, i, px_per_sector, 1 }, floor_color);
            draw_rect(state->surface->pixels, &(rect){ column * px_per_sector, WINDOW_HEIGHT - i - 1, px_per_sector, 1 }, ceiling_color);
        }
    }

    draw_debug_info(state, false);

    SDL_UpdateWindowSurface(state->window);
}

static ray_t raycast(vec2f origin, float angle, map_t* map)
{
    vec2f ray = { cosf(angle), sinf(angle) };
    vec2f step = { sqrtf(1 + powf(ray.y / ray.x, 2)), sqrtf(1 + powf(ray.x / ray.y, 2)) };
    vec2i cell = { (int32_t)origin.x, (int32_t)origin.y };

    vec2f length;
    vec2i direction;
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

static void draw_debug_info(state_t* state, bool show_advanced)
{
    char buf_fps[32];
    itoa(state->time.fps, buf_fps, 10);
    SDL_Surface* fps_text = TTF_RenderText_Blended(state->font, buf_fps, (SDL_Color){ 0xFF, 0xFF, 0xFF, 0xFF });

    int32_t w, h;
    TTF_SizeText(state->font, buf_fps, &w, &h);
    SDL_BlitSurface(fps_text, NULL, state->surface, &(rect){ 10, 10, w, h });
    SDL_FreeSurface(fps_text);

    if (show_advanced) {
        char buf_pos[32];
        snprintf(buf_pos, 32, "x: %.2f, y: %.2f", state->player.position.x, state->player.position.y);
        SDL_Surface *pos_text = TTF_RenderText_Blended(state->font, buf_pos, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF});

        TTF_SizeText(state->font, buf_pos, &w, &h);
        SDL_BlitSurface(pos_text, NULL, state->surface, &(rect){ 10, 28, w, h });
        SDL_FreeSurface(pos_text);

        char buf_ang[32];
        snprintf(buf_ang, 32, "%.3f", state->player.angle * (180.0f / M_PI));
        SDL_Surface *angle_text = TTF_RenderText_Blended(state->font, buf_ang, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF});

        TTF_SizeText(state->font, buf_ang, &w, &h);
        SDL_BlitSurface(angle_text, NULL, state->surface, &(rect){ 10, 46, w, h });
        SDL_FreeSurface(angle_text);
    }
}