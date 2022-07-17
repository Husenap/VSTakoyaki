#include "preview.hpp"

#include <imgui/imgui.h>

namespace ht {

void Preview::Update(const RenderTarget& renderTarget) {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, {0.f, 0.f, 0.f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ChildBg, {0.f, 0.f, 0.f, 1.0f});
    if (Begin("Preview")) {
        mIsHovered = ImGui::IsWindowHovered();

        const ImVec2 regionMin = ImGui::GetWindowContentRegionMin();
        const ImVec2 regionMax = ImGui::GetWindowContentRegionMax();
        const ImVec2 offset    = regionMin;
        const ImVec2 regionSize(regionMax.x - regionMin.x,
                                regionMax.y - regionMin.y);
        ImVec2 imageSize{static_cast<float>(renderTarget.GetSize().x), static_cast<float>(renderTarget.GetSize().y)};

        const float regionRatio = regionSize.x / regionSize.y;
        const float imageRatio  = static_cast<float>(renderTarget.GetSize().x) /
                                 renderTarget.GetSize().y;

        if (regionRatio > imageRatio) {
            imageSize.x *= regionSize.y / imageSize.y;
            imageSize.y = regionSize.y;
        } else {
            imageSize.y *= regionSize.x / imageSize.x;
            imageSize.x = regionSize.x;
        }

        ImGui::SetCursorPosX((regionSize.x - imageSize.x) * 0.5f + offset.x);
        ImGui::SetCursorPosY((regionSize.y - imageSize.y) * 0.5f + offset.y);

        ImGui::Image((void*)(intptr_t)renderTarget.GetRenderTexture(),
                     imageSize,
                     {0, 1},
                     {1, 0});
    }
    End();
    ImGui::PopStyleColor(2);
}

}  // namespace ht
