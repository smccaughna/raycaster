#include "global.h"
#include "render.h"

bool init_game(state_t* state)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error initializing SDL", SDL_GetError(), NULL);
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error initializing SDL_image", IMG_GetError(), NULL);
        return false;
    }

    if (TTF_Init() < 0) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error initializing SDL_ttf", TTF_GetError(), NULL);
        return false;
    }

    state->window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (state->window == NULL) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error initializing Window", SDL_GetError(), NULL);
        return false;
    }

    state->renderer = SDL_CreateRenderer(state->window, -1, SDL_RENDERER_ACCELERATED);
    if (state->renderer == NULL) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error initializing Renderer", SDL_GetError(), NULL);
        return false;
    }

    state->time.fps_max = 60;

    state->player.position = (SDL_FPoint){ 2.0f, 4.0f };
    state->player.speed = 2.0f;

    state->map.width = 8;
    state->map.height = 8;
    state->map.grid = malloc(state->map.width * state->map.height * sizeof(bool));
    memcpy_s(
        state->map.grid,
        state->map.width * state->map.height * sizeof(bool),
        (bool[]){
                1, 1, 1, 1, 1, 1, 1, 1,
                1, 0, 0, 0, 0, 0, 0, 1,
                1, 0, 0, 0, 0, 0, 0, 1,
                1, 0, 0, 1, 1, 0, 0, 1,
                1, 0, 0, 1, 1, 0, 0, 1,
                1, 0, 0, 0, 0, 0, 0, 1,
                1, 0, 0, 0, 0, 0, 0, 1,
                1, 1, 1, 1, 1, 1, 1, 1
        },
        state->map.width * state->map.height * sizeof(bool)
    );

    return true;
}

void quit_game(state_t* state)
{
    free(state->map.grid);

    if (state->window) {
        SDL_DestroyWindow(state->window);
    }

    if (state->renderer) {
        SDL_DestroyRenderer(state->renderer);
    }

    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
}