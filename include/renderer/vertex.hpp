#pragma once

#include "../misc.hpp"

template <typename T> struct vertex_attribute {};

struct VertexData {
  GLenum type;
  z components, component_size;
  bool normalized;
};

template <> struct vertex_attribute<v3> {
  static constexpr GLenum type = GL_FLOAT;
  static constexpr z components = 3;
  static constexpr z component_size = sizeof(f32);
  static constexpr z stride = components * sizeof(f32);
  static constexpr bool normalized = false;
};

template <typename... Ts> struct VertexBuilder {
  static constexpr z vertex_size = (0 + ... + sizeof(Ts));

  VertexBuilder() { raw.reserve(vertex_size * 24); }

  void add_vertex(Ts &&...attributes) {
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
