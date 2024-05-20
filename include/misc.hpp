#pragma once

#include <glm/ext/vector_int2.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#define TRY(expr)                                                              \
  {                                                                            \
    if (decltype(auto) result = expr) {                                        \
      return result;                                                           \
    }                                                                          \
  }

#define EXPECT(expr, msg, ...)                                                 \
  {                                                                            \
    if (!(expr)) {                                                             \
      spdlog::error("Expectation '" msg "' failed " __FILE__ ":%d",            \
                    __VA_ARGS__ __LINE__);                                     \
      exit(2);                                                                 \
    }                                                                          \
  }
