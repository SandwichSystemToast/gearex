#pragma once

#include <set>

#include <spdlog/spdlog.h>

#include "../misc.hpp"

struct Renderer {
  /// @returns `true` if the argument is a valid shader, `false` otherwise
  bool check_shader(gl shader) {
    EXPECT(glIsShader(shader), "Argument is a shader");

    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
      char msg[512];
      glGetShaderInfoLog(shader, 512, 0, msg);
      spdlog::error(msg);
      return false;
    }

    return true;
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

  ~Renderer() {
    for (auto program : programs)
      glDeleteShader(program);
  }

private:
  std::set<gl> programs;
};
