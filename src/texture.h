#pragma once

#include <stdint.h>
#include <SDL.h>

#define TEXTURE_SIZE 32

bool load_textures();
void unload_textures();

extern SDL_Surface* textures[4];