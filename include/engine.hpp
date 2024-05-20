#pragma once

#include "misc.hpp"

#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

static inline int initialize_globals() {
  TRY(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER));
  return 0;
}

struct GameEngine {
    GameEngine() {
        
    }
};
