#pragma once

typedef struct state state_t;

typedef struct ray
{
    vec2f point;
    vec2i cell;
    enum {
        NORTH,
        SOUTH,
        EAST,
        WEST
    } side;
} ray_t;

extern void render(state_t* state);