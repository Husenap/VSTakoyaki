#pragma once

#include <glm/glm.hpp>

namespace ht {
struct KeyInput {
    int key;
    int scancode;
    int action;
    int mods;
};

struct MouseInput {
    int button;
    int action;
    int mods;
};

struct CursorInput {
    glm::vec2 pos;
    glm::vec2 delta;
};
}  // namespace ht