#include "imgui_wrapper.hpp"

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_opengl3_loader.h>
#include <imgui/imgui.h>

#include "takoyakicontroller.h"

namespace ht::ImGuiWrapper {

void SetupStyle();

void Initialize(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    if (!std::filesystem::exists(IniFilePath)) {
        std::filesystem::create_directories(IniFilePath.parent_path());
        const auto imguiTemplatePath =
            TakoyakiController::getResourcesPath() / "imgui.ini";
        if (std::filesystem::exists(imguiTemplatePath))
            std::filesystem::copy(imguiTemplatePath, IniFilePath);
    }
    io.IniFilename = IniFileString.c_str();

    SetupStyle();
}

void Destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport();
}

void EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SetupStyle() {
    {
        const auto fontPath =
            TakoyakiController::getResourcesPath() / "Roboto-Regular.ttf";
        ImGui::GetIO().Fonts->AddFontFromFileTTF(
            fontPath.generic_string().c_str(), 16.f);
    }

    ImGuiStyle& style                      = ImGui::GetStyle();
    ImVec4*     colors                     = style.Colors;
    colors[ImGuiCol_Text]                  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]          = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_ChildBg]               = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_WindowBg]              = ImVec4(0.25f, 0.25f, 0.25f, 0.75f);
    colors[ImGuiCol_PopupBg]               = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_Border]                = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
    colors[ImGuiCol_BorderShadow]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_FrameBg]               = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
    colors[ImGuiCol_TitleBg]               = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_TitleBgActive]         = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
    colors[ImGuiCol_MenuBarBg]             = ImVec4(0.33f, 0.33f, 0.33f, 0.75f);
    colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    colors[ImGuiCol_CheckMark]             = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
    colors[ImGuiCol_SliderGrab]            = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
    colors[ImGuiCol_Button]                = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    colors[ImGuiCol_Header]                = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
    colors[ImGuiCol_HeaderActive]          = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
    colors[ImGuiCol_Separator]             = ImVec4(0.00f, 0.00f, 0.00f, 0.14f);
    colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.70f, 0.67f, 0.60f, 0.29f);
    colors[ImGuiCol_SeparatorActive]       = ImVec4(0.70f, 0.67f, 0.60f, 0.67f);
    colors[ImGuiCol_ResizeGrip]            = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered]     = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.86f, 0.66f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
    colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_Tab]                   = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_TabHovered]            = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_TabActive]             = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    colors[ImGuiCol_TabUnfocused]          = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    colors[ImGuiCol_DockingPreview]        = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);

    style.PopupRounding = 3.f;

    style.WindowPadding    = ImVec2(4.f, 4.f);
    style.FramePadding     = ImVec2(6.f, 4.f);
    style.ItemSpacing      = ImVec2(3.f, 3.f);
    style.ItemInnerSpacing = ImVec2(3.f, 3.f);

    style.ScrollbarSize = 18.f;

    style.WindowBorderSize = 1.f;
    style.ChildBorderSize  = 1.f;
    style.PopupBorderSize  = 1.f;
    style.FrameBorderSize  = 0.f;

    style.WindowRounding    = 2.f;
    style.ChildRounding     = 2.f;
    style.FrameRounding     = 2.f;
    style.ScrollbarRounding = 2.f;
    style.GrabRounding      = 3.f;

    style.TabBorderSize = 0.f;
    style.TabRounding   = 3.f;
}

}  // namespace ht::ImGuiWrapper