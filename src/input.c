#include "global.h"
#include "input.h"

void handle_input(state_t* state)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                state->should_quit = true;
                break;
        }
    }

    state->keyboard = SDL_GetKeyboardState(NULL);
    state->mouse.state = SDL_GetMouseState(&state->mouse.position.x, &state->mouse.position.y);
}