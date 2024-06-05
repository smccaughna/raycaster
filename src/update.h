#pragma once

#include <SDL.h>

typedef struct state state_t;

extern void update(state_t* state);

static inline bool check_collision(SDL_FRect* r1, SDL_FRect* r2)
{
    return  r1->x + r1->w >= r2->x &&
            r1->x <= r2->x + r2->w &&
            r1->y + r1->h >= r2->y &&
            r1->y <= r2->y + r2->h;
}