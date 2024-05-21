#pragma once

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <set>

#include <spdlog/spdlog.h>

#include "../misc.hpp"
#include "mesh.hpp"

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

  Mesh make_mesh(MeshBuilder &&builder) {
    z attributes_len = builder.vertex_attributes.size();
    std::vector<gl> vertex_buffers(attributes_len);
    gl vertex_array;

    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    glGenBuffers(attributes_len, vertex_buffers.data());
    for (z i = 0; i < attributes_len; i++) {
      auto &attribute = builder.vertex_attributes[i];
      glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[i]);
      glBufferData(GL_ARRAY_BUFFER, attribute.len, attribute.data,
                   GL_STATIC_DRAW);
      glVertexAttribPointer(i, attribute.size, attribute.type,
                            attribute.normalized, attribute.stride, (void *)0);
      glEnableVertexAttribArray(i);
    }

    return Mesh{
        .vertex_array = vertex_array,
        .vertex_buffers = vertex_buffers,
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
