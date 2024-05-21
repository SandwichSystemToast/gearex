#pragma once

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <set>
#include <span>

#include <spdlog/spdlog.h>

#include "../misc.hpp"
#include "vertex.hpp"

static inline void GLAPIENTRY glMessageCallback(GLenum source, GLenum type,
                                                GLuint id, GLenum severity,
                                                GLsizei length,
                                                const GLchar *msg,
                                                const void *_) {
  auto log_level = spdlog::level::warn;
  if (type == GL_DEBUG_TYPE_ERROR)
    log_level = spdlog::level::err;
  spdlog::log(log_level, "GL Error: {}", msg);
}

struct Mesh {
  gl vertex_array, index_buffer, vertex_buffer;
};

struct Renderer {
  void setup_opengl_debug() {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(glMessageCallback, nullptr);
  }

  [[nodiscard("Discarding a shader leaks memory")]]
  gl make_program(const char *vertex, const char *fragment,
                  const char *name = nullptr) {
    gl vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertex, 0);
    glCompileShader(vert);
    check_shader(vert);

    gl frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragment, 0);
    glCompileShader(frag);
    check_shader(frag);

    gl shader = glCreateProgram();
    glAttachShader(shader, vert);
    glAttachShader(shader, frag);
    glLinkProgram(shader);

    glDeleteShader(vert);
    glDeleteShader(frag);

    if (name) {
      glObjectLabel(GL_PROGRAM, shader,
                    std::min(strlen(name), (size_t)GL_MAX_LABEL_LENGTH), name);
    }

    programs.insert(shader);
    return shader;
  }

  template <typename... Ts>
  Mesh make_mesh(VertexBuilder<Ts...> &&vertex_builder,
                 std::span<u32> indices) {
    gl vertex_buffer, vertex_array, index_buffer;

    // Vertex Array
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    // Index Buffer
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(),
                 GL_STATIC_DRAW);

    // Vertex Buffer
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_builder.size_bytes(),
                 vertex_builder.data(), GL_STATIC_DRAW);

    z i = 0;
    z offset = 0;
    z stride = VertexBuilder<Ts...>::stride;
    (
        [&] {
          using va = vertex_attribute<Ts>;
          glVertexAttribPointer(i, va::components, va::type, va::normalized,
                                stride, (void *)(0 + offset));
          glEnableVertexAttribArray(i);
          offset += va::components * va::component_size;
          i += 1;
        }(),
        ...);

    return Mesh{
        .vertex_array = vertex_array,
        .index_buffer = index_buffer,
        .vertex_buffer = vertex_buffer,
    };
  }

  ~Renderer() {
    for (auto program : programs)
      glDeleteProgram(program);
  }

protected:
  std::set<gl> programs;

  /// @returns `true` if the argument is a valid shader, `false` otherwise
  bool check_shader(gl shader) {
    EXPECT(glIsShader(shader), "Argument is a shader");

    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
      // TODO: respect infolog length with GL_INFO_LOG_LENGTH
      char msg[512];
      glGetShaderInfoLog(shader, 512, 0, msg);
      spdlog::error(msg);
      return false;
    }

    return true;
  }
};
