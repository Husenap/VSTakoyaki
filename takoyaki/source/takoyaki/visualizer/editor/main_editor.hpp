#pragma once

#include <imgui/imgui.h>

#include "service_manager.hpp"

namespace ht {

class MainEditor {
public:
    MainEditor(Camera& camera, Preview& preview, UniformsMenu& uniformsMenu)
        : mCamera(camera)
        , mPreview(preview)
        , mUniformsMenu(uniformsMenu) {}
    ~MainEditor() = default;

    void LoadProjectFile(const std::string& fileToLoad);

    void Update(float         deltaTime,
                bool          hasProjectLoaded,
                RenderTarget& renderTarget);
    void RegisterCommands(RenderCommandList<RenderCommand>& cmds,
                          const ShaderProgram&              program);

    void OnInput(const KeyInput& input);
    void OnInput(const MouseInput& input);
    void OnInput(const CursorInput& input);

    void OnFramebufferSize(const glm::ivec2& size);

    void ReportError(const std::string& message);

    void SetNewFileHandler(NewFileHandler handler) {
        mNewFileHandler = handler;
    }
    void SetOpenFileHandler(OpenFileHandler handler) {
        mOpenFileHandler = handler;
    }
    void SetSaveFileHandler(SaveFileHandler handler) {
        mSaveFileHandler = handler;
    }

private:
    void DisplayErrors();

    bool mShowDemoWindow = false;
    bool mCameraMode     = false;

    ImVec2                   mMenuBarSize{};
    glm::ivec2               mFramebufferSize{};
    std::vector<std::string> mErrors{};

    UniformsMenu& mUniformsMenu;
    Preview&      mPreview;
    Camera&       mCamera;

    OpenFileHandler mNewFileHandler;
    OpenFileHandler mOpenFileHandler;
    SaveFileHandler mSaveFileHandler;
};

}  // namespace ht