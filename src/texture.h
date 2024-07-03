#pragma once

#include <SDL.h>

#define TEXTURE_SIZE 32
#define NUM_TEXTURES 4

typedef struct state state_t;

bool load_textures(SDL_Surface** textures);
void unload_textures(SDL_Surface** textures);