#pragma once

typedef struct state state_t;

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

extern void render(state_t* state);