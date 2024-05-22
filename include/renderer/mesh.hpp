#pragma once

#include "misc.hpp"
#include <GL/glcorearb.h>

enum struct Topology : u8 { TRIANGLES, TRIANGLE_STRIP, QUADS, LINES };

/// VAO + EBO + VBO + DrawMode
struct Mesh {
  u32 index_count;
  gl vertex_array, index_buffer, vertex_buffer;
  Topology topology;

  GLenum gl_draw_mode() {
    switch (topology) {
    case Topology::TRIANGLES:
      return GL_TRIANGLES;
    case Topology::TRIANGLE_STRIP:
      return GL_TRIANGLE_STRIP;
    case Topology::QUADS:
      return GL_QUADS;
    case Topology::LINES:
      return GL_LINES;
    }
    PANIC("Undefined draw topology {}", (u8)topology);
  }
};
