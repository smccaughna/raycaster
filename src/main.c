#include "global.h"
#include "input.h"
#include "update.h"
#include "render.h"

int main(int argc, char** argv) {
    state_t state = { 0 };
    if (!init_game(&state)) {
        return 1;
    }

    while (!state.should_quit) {
        uint64_t last = SDL_GetTicks64();
        state.time.tick++;

        handle_input(&state);
        update(&state);
        render(&state);

        state.time.delta = ((SDL_GetTicks64() - last) / 1000.0f);
    }

    quit_game(&state);

    return 0;
}