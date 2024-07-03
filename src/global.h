#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <math.h>

#define SDL_MAIN_HANDLED

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

typedef struct mouse
{
    SDL_Point position;
    uint32_t state;
} mouse_t;

typedef struct time
{
    uint64_t tick;
    uint64_t last;
    float delta;
    SDL_TimerID fps_timer;
    int32_t fps;
    int32_t fps_max;
} clock_t;

typedef struct player
{
    SDL_FPoint position;
    float speed;
    float angle;
} player_t;

typedef struct map
{
    uint8_t* grid;
    int32_t width, height;
} map_t;

typedef struct state
{
    bool should_quit;
    SDL_Window* window;
    SDL_Surface* surface;
    TTF_Font* font;
    uint8_t* keyboard;
    mouse_t mouse;
    clock_t time;
    player_t player;
    map_t map;
} state_t;

extern bool init_game(state_t* state);
extern void quit_game(state_t* state);

extern uint32_t calculate_fps(uint32_t interval, void* param);