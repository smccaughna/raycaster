#include "global.h"
#include "texture.h"

bool init_game(state_t* state)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error initializing SDL", SDL_GetError(), NULL);
        return false;
    }

    state->window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (state->window == NULL) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error initializing Window", SDL_GetError(), NULL);
        return false;
    }

    state->surface = SDL_GetWindowSurface(state->window);
    if (state->surface == NULL) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error getting window surface", SDL_GetError(), NULL);
        return false;
    }

    state->time.fps_max = 60;

    state->player.position = (SDL_FPoint){ 4.5f, 4.5f };
    state->player.speed = 2.0f;

    state->map.width = 9;
    state->map.height = 9;
    state->map.grid = malloc(state->map.width * state->map.height * sizeof(bool));
    memcpy_s(
        state->map.grid,
        state->map.width * state->map.height * sizeof(uint8_t),
        (uint8_t[]){
                1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 0, 0, 0, 0, 0, 0, 0, 1,
                1, 0, 3, 3, 0, 2, 2, 0, 1,
                1, 0, 3, 0, 0, 0, 2, 0, 1,
                1, 0, 0, 0, 0, 0, 0, 0, 1,
                1, 0, 2, 0, 0, 0, 3, 0, 1,
                1, 0, 2, 2, 0, 3, 3, 0, 1,
                1, 0, 0, 0, 0, 0, 0, 0, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1
        },
        state->map.width * state->map.height * sizeof(bool)
    );

    if (!load_textures()) {
        return false;
    }

    return true;
}

void quit_game(state_t* state)
{
    free(state->map.grid);

    unload_textures();

    if (state->surface) {
        SDL_FreeSurface(state->surface);
    }

    if (state->window) {
        SDL_DestroyWindow(state->window);
    }

    SDL_Quit();
}