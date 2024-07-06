#include "global.h"
#include "render.h"
#include "texture.h"

#define FOV M_PI_2
#define PLANE_WIDTH (tanf(FOV / 2.0f) * 2.0f)
#define NUM_SECTORS 192

static void draw_debug_info(state_t* state, bool show_advanced);
static void render_line(state_t* state, int32_t line);
static ray_t raycast(SDL_FPoint origin, float angle, map_t* map);

static void draw_rect(uint32_t* pixels, SDL_Rect* rect, uint32_t color)
{
    for (int32_t j = rect->y; j < rect->y + rect->h; j++) {
        for (int32_t i = rect->x; i < rect->x + rect->w; i++) {
            memcpy(pixels + (j * WINDOW_WIDTH) + i, &color, sizeof(uint32_t));
        }
    }
}

void render(state_t* state)
{
    draw_rect(state->surface->pixels, &(SDL_Rect){ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT / 2 }, 0xFF808080);
    draw_rect(state->surface->pixels, &(SDL_Rect){ 0, WINDOW_HEIGHT / 2, WINDOW_WIDTH, WINDOW_HEIGHT / 2 }, 0xFF000000);

    for (int i = 0; i < NUM_SECTORS; i++) {
        render_line(state, i);
    }

    draw_debug_info(state, true);

    SDL_UpdateWindowSurface(state->window);
}

static void render_line(state_t* state, int32_t line)
{
    float angle = state->player.angle + atanf((line * (PLANE_WIDTH / NUM_SECTORS)) - (PLANE_WIDTH / 2.0f));

    ray_t ray = raycast(state->player.position, angle, &state->map);
    float distance = sqrtf(powf(state->player.position.x - ray.point.x, 2) + powf(state->player.position.y - ray.point.y, 2));
    int32_t wall_height = (int32_t)(WINDOW_HEIGHT / (distance * cosf(angle - state->player.angle)));

    float wall_x = (ray.side == NORTH || ray.side == SOUTH)
                   ? fmodf(ray.point.x, ray.block.x)
                   : fmodf(ray.point.y, ray.block.y);

    int32_t tex_x = (int32_t)(wall_x * TEXTURE_SIZE);
    if (ray.side == NORTH || ray.side == EAST)
        tex_x = TEXTURE_SIZE - tex_x - 1;

    int32_t wall_start = (WINDOW_HEIGHT - wall_height) / 2;
    int32_t wall_end = (WINDOW_HEIGHT + wall_height) / 2;

    float step = TEXTURE_SIZE / (float)wall_height;
    float tex_pos = (float)(wall_start - (WINDOW_HEIGHT / 2) + (wall_height / 2)) * step;

    uint32_t* texture = (uint32_t*)state->textures[state->map.grid[ray.block.y * state->map.width + ray.block.x] - 1]->pixels;

    for (int32_t i = wall_start; i < wall_end; i++) {
        int32_t tex_y = (int32_t)tex_pos;
        tex_pos += step;

        if (i > WINDOW_HEIGHT - 1 || i < 0) continue;

        uint32_t color = texture[tex_y * TEXTURE_SIZE + tex_x];

        if (ray.side == EAST || ray.side == WEST)
            color = 0xFF000000 | ((uint32_t)((color >> 16 & 0xFF) * 0.75f) << 16)
                               | ((uint32_t)((color >> 8  & 0xFF) * 0.75f) << 8)
                               | ((uint32_t)((color       & 0xFF) * 0.75f));

        color = 0xFF000000 | ((uint32_t)((color >> 16 & 0xFF) * (1.0f / (distance * 0.3f + 1))) << 16)
                           | ((uint32_t)((color >> 8  & 0xFF) * (1.0f / (distance * 0.3f + 1))) << 8)
                           | ((uint32_t)((color       & 0xFF) * (1.0f / (distance * 0.3f + 1))));

        draw_rect(state->surface->pixels, &(SDL_Rect){ line * (WINDOW_WIDTH / NUM_SECTORS), i, WINDOW_WIDTH / NUM_SECTORS, 1 }, color);
    }

    SDL_FPoint floor = { 0.0f };
    switch (ray.side) {
        case NORTH:
            floor = (SDL_FPoint){ ray.point.x, ray.block.y };
            break;
        case SOUTH:
            floor = (SDL_FPoint){ ray.point.x, ray.block.y + 1.0f };
            break;
        case EAST:
            floor = (SDL_FPoint){ ray.block.x + 1.0f, ray.point.y };
            break;
        case WEST:
            floor = (SDL_FPoint){ ray.block.x, ray.point.y };
            break;
    }

    uint32_t* floor_texture = (uint32_t*)state->textures[3]->pixels;
    uint32_t* ceiling_texture = (uint32_t*)state->textures[1]->pixels;

    float wall_distance = distance * cosf(angle - state->player.angle);

    for (int j = wall_end; j < WINDOW_HEIGHT; j++) {
        float current_floor_distance = WINDOW_HEIGHT / (2.0f * j - WINDOW_HEIGHT);
        float weight = current_floor_distance / wall_distance;

        SDL_FPoint current_floor = {
                weight * floor.x + (1.0f - weight) * state->player.position.x,
                weight * floor.y + (1.0f - weight) * state->player.position.y
        };

        SDL_Point tex = {
                (int32_t)(current_floor.x * TEXTURE_SIZE) % TEXTURE_SIZE,
                (int32_t)(current_floor.y * TEXTURE_SIZE) % TEXTURE_SIZE
        };

        uint32_t floor_color = floor_texture[tex.y * TEXTURE_SIZE + tex.x];
        uint32_t ceiling_color = ceiling_texture[tex.y * TEXTURE_SIZE + tex.x];

        floor_color = 0xFF000000 | ((uint32_t)((floor_color >> 16 & 0xFF) * (1.0f / (current_floor_distance * 0.3f + 1))) << 16)
                                 | ((uint32_t)((floor_color >> 8  & 0xFF) * (1.0f / (current_floor_distance * 0.3f + 1))) << 8)
                                 | ((uint32_t)((floor_color       & 0xFF) * (1.0f / (current_floor_distance * 0.3f + 1))));

        ceiling_color = 0xFF000000 | ((uint32_t)((ceiling_color >> 16 & 0xFF) * (1.0f / (current_floor_distance * 0.3f + 1))) << 16)
                                   | ((uint32_t)((ceiling_color >> 8  & 0xFF) * (1.0f / (current_floor_distance * 0.3f + 1))) << 8)
                                   | ((uint32_t)((ceiling_color       & 0xFF) * (1.0f / (current_floor_distance * 0.3f + 1))));

        draw_rect(state->surface->pixels, &(SDL_Rect){ line * (WINDOW_WIDTH / NUM_SECTORS), j, WINDOW_WIDTH / NUM_SECTORS, 1 }, floor_color);
        draw_rect(state->surface->pixels, &(SDL_Rect){ line * (WINDOW_WIDTH / NUM_SECTORS), WINDOW_HEIGHT - j - 1, WINDOW_WIDTH / NUM_SECTORS, 1 }, ceiling_color);
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

static void draw_debug_info(state_t* state, bool show_advanced)
{
    char buf_fps[5];
    itoa(state->time.fps, buf_fps, 10);
    SDL_Surface* fps_text = TTF_RenderText_Blended(state->font, buf_fps, (SDL_Color){ 0xFF, 0xFF, 0xFF, 0xFF });

    int32_t w, h;
    TTF_SizeText(state->font, buf_fps, &w, &h);
    SDL_BlitSurface(fps_text, NULL, state->surface, &(SDL_Rect){ 10, 10, w, h });
    SDL_FreeSurface(fps_text);

    if (show_advanced) {
        char buf_pos[19];
        snprintf(buf_pos, 19, "x: %.2f, y: %.2f", state->player.position.x, state->player.position.y);
        SDL_Surface *pos_text = TTF_RenderText_Blended(state->font, buf_pos, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF});

        TTF_SizeText(state->font, buf_pos, &w, &h);
        SDL_BlitSurface(pos_text, NULL, state->surface, &(SDL_Rect) {10, 28, w, h});
        SDL_FreeSurface(pos_text);

        char buf_ang[8];
        snprintf(buf_ang, 8, "%.3f", state->player.angle * (180.0f / M_PI));
        SDL_Surface *angle_text = TTF_RenderText_Blended(state->font, buf_ang, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF});

        TTF_SizeText(state->font, buf_ang, &w, &h);
        SDL_BlitSurface(angle_text, NULL, state->surface, &(SDL_Rect) {10, 46, w, h});
        SDL_FreeSurface(angle_text);
    }
}