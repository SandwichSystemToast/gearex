#pragma once

#define GL_GLEXT_PROTOTYPES
#include <GL/glcorearb.h>

#include <type_traits>

#include "engine/misc.hpp"
#include "geometry.hpp"

namespace engine::renderer {

/// VAO + EBO + VBO + DrawMode
struct Mesh {
  u32 index_count;
  gl vertex_array, index_buffer, vertex_buffer;
  glenum draw_mode;
};

static_assert(std::is_trivial_v<Mesh>);

} // namespace engine::renderer
