#pragma once

#include <GL/gl.h>

#include <GL/glext.h>
#include <SDL2/SDL.h>

#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include "input.hpp"
#include "window.hpp"

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
