#pragma once

typedef struct state state_t;

extern void render(state_t* state);
extern void render_map(state_t*, SDL_Point position, int32_t scale);