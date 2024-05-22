#pragma once

#include <vector>

#include "renderer/mesh.hpp"

enum struct CommandKind { DRAW_MESH };

struct Command {
  CommandKind kind;
  union {
    struct {
      Mesh mesh;
      gl shader_program;
    } draw_mesh;
  };
};

static_assert(std::is_trivial_v<Command>);

struct CommandQueue {
  CommandQueue() {}

  CommandQueue &draw_mesh(Mesh mesh, gl shader_program) {
    auto command = Command{
        .kind = CommandKind::DRAW_MESH,
        .draw_mesh = {.mesh = mesh, .shader_program = shader_program},
    };

    queue.push_back(command);
    return *this;
  };

  std::vector<Command> queue;
};
