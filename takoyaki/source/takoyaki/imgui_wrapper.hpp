#pragma once

#include <filesystem>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ht::ImGuiWrapper {

static const auto IniFilePath =
    std::filesystem::temp_directory_path() / "VSTakoyaki" / "imgui.ini";
static const auto IniFileString = IniFilePath.generic_string();

void Initialize(GLFWwindow* window);
void Destroy();

void BeginFrame();
void EndFrame();

}  // namespace ht::ImGuiWrapper