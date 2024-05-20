#pragma once

#include <SDL2/SDL.h>

#include <glm/ext/vector_int2.hpp>
#include <glm/glm.hpp>

#include <spdlog/spdlog.h>

#include <map>

enum struct InputState {
  NONE = 0,
  PRESSED = 1,
  HELD = 2,
  RELEASED = 3,
};

struct Input {
  Input() {}

  glm::ivec2 get_relative_mouse_position() {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return glm::ivec2(x, y);
  }

  glm::ivec2 get_vector(SDL_Keycode neg_x, SDL_Keycode pos_x, SDL_Keycode neg_y,
                        SDL_Keycode pos_y) const {
    auto nx = is_key_active(neg_x);
    auto px = is_key_active(pos_x);
    auto ny = is_key_active(neg_y);
    auto py = is_key_active(pos_y);

    return glm::ivec2(px - nx, py - ny);
  }

  bool is_key_active(SDL_Keycode keycode) const {
    auto key_iter = states.find(keycode);
    if (key_iter != states.end())
      return key_iter->second != InputState::NONE;
    return false;
  }

  void update(Uint8 key_state, SDL_Keycode keycode) {
    states[(int)keycode] =
        key_state == SDL_PRESSED ? InputState::PRESSED : InputState::RELEASED;
  }

  void tick() {
    for (auto &[key, state] : states) {
      switch (state) {
      case InputState::NONE:
      case InputState::HELD:
        break;
      case InputState::PRESSED:
        state = InputState::HELD;
        break;
      case InputState::RELEASED:
        state = InputState::NONE;
        break;
      }
    }
  }

  std::map<int, InputState> states;
};
