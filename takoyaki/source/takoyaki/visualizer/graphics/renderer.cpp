#include "renderer.hpp"

namespace ht {

void Renderer::ProcessCommands() {
    mCommandList.Visit(RenderCommandExecutor());
}

RenderCommandList<RenderCommand>& Renderer::Commands() {
    return mCommandList;
}

}  // namespace ht