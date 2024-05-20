#pragma once

#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

#include "input.hpp"
#include "misc.hpp"
#include "window.hpp"

static inline int initialize_globals() {
  TRY(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER));
  return 0;
}

struct Engine {
  Engine() : done(false), input(), window() {}

  void process_events() {
    input.tick();

    while (auto event = window.poll_sdl_event()) {
      switch (event->type) {
      case SDL_QUIT:
        done = true;
        break;
      case SDL_WINDOWEVENT:
        done = event->window.event == SDL_WINDOWEVENT_CLOSE;
        break;
      case SDL_KEYDOWN:
      case SDL_KEYUP:
        input.update(event->key.state, event->key.keysym.sym);
        break;
      }
    }
  }

  bool done;
  Input input;
  Window window;
};
