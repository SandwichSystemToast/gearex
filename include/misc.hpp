#include <spdlog/spdlog.h>

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
