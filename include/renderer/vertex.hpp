#pragma once

#include "../misc.hpp"

#include <span>

template <typename T> struct vertex_attribute {};

struct VertexData {
  void *data;
  GLenum type;
  z len, size, stride;
  bool normalized;
};

template <> struct vertex_attribute<v3> {
  static constexpr GLenum type = GL_FLOAT;
  static constexpr z size = 3;
  static constexpr z stride = 3 * sizeof(f32);
  static constexpr bool normalized = false;
};

struct VertexAttributeBuilder {
  template <typename T> void add_attribute(std::span<T> contents) {
    auto attribute = VertexData{
        .data = contents.data(),
        .type = vertex_attribute<T>::type,
        .len = contents.size() * sizeof(T),
        .size = vertex_attribute<T>::size,
        .stride = vertex_attribute<T>::stride,
        .normalized = vertex_attribute<T>::normalized,
    };

    vertex_attributes.push_back(attribute);
  }

  std::vector<VertexData> &&attributes() && {
    return std::move(vertex_attributes);
  }

private:
  std::vector<VertexData> vertex_attributes;
};
