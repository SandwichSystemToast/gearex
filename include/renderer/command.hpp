#pragma once

#include <vector>

enum struct CommandKind {
  
};

struct Command {};

struct CommandQueue {
  CommandQueue() {}

private:
  std::vector<Command> queue;
};
