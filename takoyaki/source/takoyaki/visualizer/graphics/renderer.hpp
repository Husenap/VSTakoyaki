#pragma once

#include "gl/render_command.hpp"

namespace ht {

class Renderer {
public:
    void                              ProcessCommands();
    RenderCommandList<RenderCommand>& Commands();

private:
    RenderCommandList<RenderCommand> mCommandList;
};

}  // namespace ht