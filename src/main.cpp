#include <spdlog/spdlog.h>

#include "engine.hpp"
#include "misc.hpp"

int main(int argc, char *argv[]) {
  TRY(initialize_globals());

  auto engine = Engine();

  while (!engine.done) {
    engine.process_events();

    engine.window.begin_frame();
    ImGui::ShowDemoWindow(nullptr);
    auto wasd = engine.input.get_vector('a', 'd', 's', 'w');
    spdlog::info("{} {}", wasd.x, wasd.y);
    auto mouse = engine.input.get_relative_mouse_position();
    spdlog::info("{} {}", mouse.x, mouse.y);
    engine.window.end_frame();
  }

  return 0;
}
