#include "widget.hpp"

#include <imgui/imgui.h>

namespace ht {

Widget::Widget() {}

Widget::~Widget() {}

bool Widget::Begin(const char* title, int flags) {
    if (!mVisibility) {
        mShouldCallEnd = false;
        return false;
    }
    mShouldCallEnd = true;

    bool visible = ImGui::Begin(title, &mVisibility, flags);

    if (visible) {
        mIsFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
    }

    return visible;
}

void Widget::End() {
    if (!mShouldCallEnd) {
        return;
    }

    ImGui::End();
}

}  // namespace ht
