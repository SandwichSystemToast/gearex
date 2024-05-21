#pragma once

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glcorearb.h>
#include <GL/glext.h>

#include <glm/ext/vector_int2.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#define PANIC(msg, ...)                                                        \
  {                                                                            \
    spdlog::critical(__FILE__ ":{} " msg, __LINE__, ##__VA_ARGS__);             \
    exit(2);                                                                   \
  }

#define EXPECT(expr, msg, ...)                                                 \
  {                                                                            \
    if (!(expr)) {                                                             \
      PANIC(msg, ##__VA_ARGS__)                                                  \
    }                                                                          \
  }

using gl = GLuint;
using v2 = glm::vec2;
using v3 = glm::vec3;
using v4 = glm::vec4;
using vi2 = glm::ivec2;
using vi3 = glm::ivec3;
using vi4 = glm::ivec4;
using m4x4 = glm::mat4x4;
using m3x3 = glm::mat3x3;
using m2x2 = glm::mat2x2;

using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long;
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long;

using f32 = float;

using z = size_t;
