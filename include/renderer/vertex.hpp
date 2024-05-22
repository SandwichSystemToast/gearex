#pragma once

#define GL_GLEXT_PROTOTYPES
#include <GL/glcorearb.h>

#include <type_traits>

#include "misc.hpp"

template <typename T>
  requires std::is_standard_layout_v<T>
struct vertex_attribute {};

#define _IMPL_VERTEX_ATTRIBUTE(T, gl_type, n, size)                            \
  template <> struct vertex_attribute<T> {                                     \
    static constexpr GLenum type = gl_type;                                    \
    static constexpr z components = n;                                         \
    static constexpr z component_size = size;                                  \
  };

_IMPL_VERTEX_ATTRIBUTE(v3, GL_FLOAT, 3, sizeof(f32));
_IMPL_VERTEX_ATTRIBUTE(v2, GL_FLOAT, 2, sizeof(f32));

template <typename... Ts> struct VertexBuilder {
  static constexpr z vertex_size = (0 + ... + sizeof(Ts));
  static constexpr z stride = (0 + ... +
                               (vertex_attribute<Ts>::components *
                                vertex_attribute<Ts>::component_size));
  static_assert(
      vertex_size == stride,
      "Vertex size on the CPU does not match stride, possibly an error?");

  VertexBuilder() { raw.reserve(vertex_size * 24); }

  void add_vertex(Ts... attributes) {
    raw.reserve((raw.size() + 1) * vertex_size);

    (
        [&] {
          using va = vertex_attribute<Ts>;
          z len = va::components * va::component_size;
          auto ptr = reinterpret_cast<u8 *>(&attributes);
          for (z j = 0; j < len; j++)
            raw.push_back(ptr[j]);
        }(),
        ...);
  }

  void *data() { return raw.data(); }

  z size_bytes() { return raw.size(); }

private:
  std::vector<u8> raw;
};
