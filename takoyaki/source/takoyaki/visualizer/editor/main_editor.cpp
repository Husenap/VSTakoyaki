#include "main_editor.hpp"

#include "widgets/camera.hpp"
#include "widgets/preview.hpp"
#include "widgets/uniforms_menu.hpp"

namespace ht {

const char* ErrorPopupName = "Error##Popup";

void MainEditor::LoadProjectFile(const std::string& fileToLoad) {
    mUniformsMenu.OpenFile(fileToLoad);
    mCamera.Reset();
}

void MainEditor::Update(float         deltaTime,
                        bool          hasProjectLoaded,
                        RenderTarget& renderTarget) {
    if (mShowDemoWindow) {
        ImGui::ShowDemoWindow(&mShowDemoWindow);
    }

    if (ImGui::BeginMainMenuBar()) {
        mMenuBarSize = ImGui::GetWindowSize();
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New", "Ctrl + N")) {
                mNewFileHandler();
            }
            if (ImGui::MenuItem("Open", "Ctrl + O")) {
                mOpenFileHandler();
            }
            if (ImGui::MenuItem(
                    "Save", "Ctrl + S", nullptr, hasProjectLoaded)) {
                mSaveFileHandler();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Uniforms", "F1", nullptr, hasProjectLoaded)) {
                mUniformsMenu.ToggleVisibility();
            }
            if (ImGui::MenuItem("Preview", "F2", nullptr, hasProjectLoaded)) {
                mPreview.ToggleVisibility();
            }
            if (ImGui::MenuItem("Camera", "F3", nullptr, hasProjectLoaded)) {
                mCamera.ToggleVisibility();
            }
            if (ImGui::MenuItem("ImGui Demo Window", "F4")) {
				mShowDemoWindow = !mShowDemoWindow;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (hasProjectLoaded) {
        mUniformsMenu.Update();
        mPreview.Update(renderTarget);
        mCamera.Update(deltaTime);
    }

    if (!mErrors.empty()) {
        DisplayErrors();
    }
}

void MainEditor::RegisterCommands(RenderCommandList<RenderCommand>& cmds,
                                  const ShaderProgram&              program) {
    mUniformsMenu.RegisterCommands(cmds, program);
}

void MainEditor::OnInput(const KeyInput& input) {
    mCamera.ProcessKeyInput(input);

    if (mCameraMode) {
        return;
    }

    if (input.key == GLFW_KEY_N && input.mods == GLFW_MOD_CONTROL) {
        if (mNewFileHandler) mNewFileHandler();
    }
    if (input.key == GLFW_KEY_O && input.mods == GLFW_MOD_CONTROL) {
        if (mOpenFileHandler) mOpenFileHandler();
    }
    if (input.key == GLFW_KEY_S && (input.mods == GLFW_MOD_CONTROL)) {
        if (mSaveFileHandler) mSaveFileHandler();
    }

    if (input.key == GLFW_KEY_F1 && input.action == GLFW_PRESS) {
        mUniformsMenu.ToggleVisibility();
    }
    if (input.key == GLFW_KEY_F2 && input.action == GLFW_PRESS) {
        mPreview.ToggleVisibility();
    }
    if (input.key == GLFW_KEY_F3 && input.action == GLFW_PRESS) {
        mCamera.ToggleVisibility();
    }
    if (input.key == GLFW_KEY_F4 && input.action == GLFW_PRESS) {
        mShowDemoWindow = !mShowDemoWindow;
    }
}

void MainEditor::OnInput(const MouseInput& input) {
    if (input.button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (mPreview.IsHovered() && input.action == GLFW_PRESS) {
            mCameraMode = true;
            mCamera.SetActive(true);
        } else if (input.action == GLFW_RELEASE) {
            mCameraMode = false;
            mCamera.SetActive(false);
        }
    }
}

void MainEditor::OnInput(const CursorInput& input) {
    if (mCameraMode) {
        mCamera.ProcessMouseMovement(input.delta);
    }
}

void MainEditor::OnFramebufferSize(const glm::ivec2& size) {
    mFramebufferSize = size;
}

void MainEditor::ReportError(const std::string& message) {
    std::cout << message << std::endl;
}

void MainEditor::DisplayErrors() {
    if (!ImGui::IsPopupOpen(ErrorPopupName)) {
        ImGui::OpenPopup(ErrorPopupName);
    }
    if (ImGui::BeginPopupModal(
            ErrorPopupName, nullptr, ImGuiWindowFlags_NoResize)) {
        ImGui::SetWindowSize({300.f, 0.f});
        ImGui::TextWrapped(mErrors.front().c_str());
        if (ImGui::Button("Ok", {-FLT_EPSILON, 0.0})) {
            mErrors.erase(mErrors.begin());
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

}  // namespace ht