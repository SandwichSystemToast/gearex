#include <spdlog/spdlog.h>

#include "misc.hpp"
#include "engine.hpp"
#include "window.hpp"

int main(int argc, char *argv[]) {
  TRY(initialize_globals());

  spdlog::info("Hello, world!");
  auto window = Window();

  bool done = false;

  while (!done) {
    while (auto event = window.poll_sdl_event()) {
      if (event->type == SDL_QUIT)
        done = true;
      if (event->type == SDL_WINDOWEVENT &&
          event->window.event == SDL_WINDOWEVENT_CLOSE)
        done = true;
    }

    window.begin_frame();
    ImGui::ShowDemoWindow(nullptr);
    window.end_frame();
  }

  return 0;
}
