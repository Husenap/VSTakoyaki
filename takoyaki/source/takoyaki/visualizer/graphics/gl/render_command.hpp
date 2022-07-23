#pragma once

#include <stdexcept>
#include <vector>

#include "render_commands.hpp"

namespace ht {

using RenderCommand = std::variant<std::monostate,
                                   Commands::Viewport,
                                   Commands::ClearColor,
                                   Commands::Clear,
                                   Commands::UseProgram,
                                   Commands::BindVertexArray,
                                   Commands::VertexAttribPointer,
                                   Commands::EnableVertexAttribArray,
                                   Commands::DrawArrays,
                                   Commands::Uniform,
                                   Commands::BindFramebuffer>;

template <typename RenderCommand>
struct ImmediateRenderCommand {
    template <typename... Args>
    ImmediateRenderCommand(Args&&... args) {
        RenderCommand cmd(std::forward<Args>(args)...);
        cmd.Apply();
    }
};

struct RenderCommandExecutor {
    template <typename T>
    void operator()(T& command) const {
        command.Apply();
    }

    void operator()(std::monostate&) const {
        throw std::runtime_error(
            "Should there be a monostate in the commandlist?");
    }
};

template <typename T>
class RenderCommandList {
public:
    template <typename CommandType, typename... Args>
    void Push(Args&&... args) {
        mCommandList.emplace_back(std::in_place_type_t<CommandType>{},
                                  std::forward<Args>(args)...);
    }

    void Clear() { mCommandList.clear(); }

    template <typename Visitor>
    void Visit(Visitor&& visitor) {
        for (auto& command : mCommandList) {
            std::visit(visitor, command);
        }
    }

private:
    std::vector<T> mCommandList;
};
}  // namespace ht