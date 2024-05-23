#pragma once

#define GL_GLEXT_PROTOTYPES
#include <GL/glcorearb.h>

#include <type_traits>

#include "engine/misc.hpp"

namespace engine::renderer {

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

enum struct GeometryPrimitive : u8 { TRIANGLES, TRIANGLE_STRIP, QUADS, LINES };

struct Geometry {
  Geometry(GeometryPrimitive primitive, std::vector<u8> vertices,
           std::vector<u32> indices)
      : _primitive(primitive), _vertices(vertices), _indices(indices) {}

  const std::vector<u8> &vertices() const { return _vertices; }

  const std::vector<u32> &indices() const { return _indices; }

  const GeometryPrimitive primitive() const { return _primitive; }

protected:
  GeometryPrimitive _primitive;
  std::vector<u8> _vertices;
  std::vector<u32> _indices;
};

template <typename... Vs> struct VertexGeometry : public Geometry {
  using Geometry::Geometry;

  static constexpr z vertex_size = (0 + ... + sizeof(Vs));
  static constexpr z stride = (0 + ... +
                               (vertex_attribute<Vs>::components *
                                vertex_attribute<Vs>::component_size));
};

template <typename... Vs> struct GeometryBuilder {
  static constexpr z vertex_size = VertexGeometry<Vs...>::vertex_size;
  static constexpr z stride = VertexGeometry<Vs...>::stride;

  static_assert(
      vertex_size == stride,
      "Vertex size on the CPU does not match stride, possibly an error?");

  GeometryBuilder() { raw_vertex_data.reserve(vertex_size * 24); }

  void add_vertex(Vs... attributes) {
    raw_vertex_data.reserve((raw_vertex_data.size() + 1) * vertex_size);

    (
        [&] {
          using va = vertex_attribute<Vs>;
          z len = va::components * va::component_size;
          auto ptr = reinterpret_cast<u8 *>(&attributes);
          for (z j = 0; j < len; j++)
            raw_vertex_data.push_back(ptr[j]);
        }(),
        ...);
  }

  void *data() { return raw_vertex_data.data(); }

  VertexGeometry<Vs...> stitch(GeometryPrimitive primitive,
                               std::span<u32> indices) {
    return VertexGeometry<Vs...>(
        primitive,
        std::vector<u8>(raw_vertex_data.begin(), raw_vertex_data.end()),
        std::vector<u32>(indices.begin(), indices.end()));
  }

  z size_bytes() { return raw_vertex_data.size(); }

protected:
  std::vector<u8> raw_vertex_data;
};

} // namespace engine::renderer
