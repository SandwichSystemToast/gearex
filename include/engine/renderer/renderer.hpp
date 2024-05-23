#pragma once

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <set>
#include <span>

#include <spdlog/spdlog.h>

#include "../misc.hpp"
#include "command.hpp"
#include "vertex.hpp"

namespace engine::renderer {

static inline void GLAPIENTRY glMessageCallback(GLenum source, GLenum type,
                                                GLuint id, GLenum severity,
                                                GLsizei length,
                                                const GLchar *msg,
                                                const void *_) {
  auto log_level = spdlog::level::warn;
  if (type == GL_DEBUG_TYPE_ERROR)
    log_level = spdlog::level::err;
  spdlog::log(log_level, "{}", msg);
}

struct Renderer {
  void setup_opengl_debug() {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(glMessageCallback, nullptr);
  }

  void submit_queue(CommandQueue queue) {
    for (auto command : queue.queue) {
      switch (command.kind) {
      case CommandKind::DRAW_MESH: {
        Mesh mesh = command.draw_mesh.mesh;
        glUseProgram(command.draw_mesh.shader_program);
        glBindVertexArray(mesh.vertex_array);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.index_buffer);
        glDrawElements(mesh.gl_draw_mode(), mesh.index_count, GL_UNSIGNED_INT,
                       0);
        glBindVertexArray(0);
        break;
      }
      default:
        PANIC("Undefined command kind");
      }
    }
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

  gl make_texture(u8 *data, u32 width, u32 height) {
    EXPECT(data != nullptr, "Supplied texture was non-existent");

    gl texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    return texture;
  }

  template <typename... Ts>
  Mesh make_mesh(VertexBuilder<Ts...> &&vertex_builder, std::span<u32> indices,
                 Topology topology) {
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
          glVertexAttribPointer(i, va::components, va::type, GL_FALSE, stride,
                                (void *)(0 + offset));
          glEnableVertexAttribArray(i);
          offset += va::components * va::component_size;
          i += 1;
        }(),
        ...);

    return Mesh{
        .index_count = (u32)indices.size(),
        .vertex_array = vertex_array,
        .index_buffer = index_buffer,
        .vertex_buffer = vertex_buffer,
        .topology = topology,
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
    EXPECT(glIsShader(shader), "glObject {} is not a shader", shader);

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

} // namespace engine::renderer
