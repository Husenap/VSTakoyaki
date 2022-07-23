#pragma once

#include "service_manager.hpp"

namespace ht {

class Takoyaki {
public:
    Takoyaki(MainWindow&       window,
             Visualizer const& visualizer,
             Renderer&         renderer,
             FileWatcher&      fileWatcher,
             MainEditor&       editor,
             Camera&           camera,
             UniformsMenu&     uniformsMenu);
    ~Takoyaki();

    void Tick();

private:
    void OnInput(const KeyInput& input);
    void OnInput(const MouseInput& input);
    void OnInput(const CursorInput& input);

    void OnFramebufferSize(const glm::ivec2& size);
    void CreateVertexBuffer();
    void CreateRenderTarget();
    void SetupListeners();

    void LoadShader();

    void OnNewFile();
    void OnOpenFile();
    void OnSaveFile();

    void OnUniformsChanged();

    void LoadProjectFile(const char* fileToLoad);

    std::string mCurrentProject{};
    float       mCurrentTime{};
    glm::vec2   mFFTLow{};
    glm::vec2   mFFTMid{};
    glm::vec2   mFFTHigh{};

    GLuint                         mVertexArray;
    std::unique_ptr<ShaderProgram> mProgram;
    GLint                          mPosLoc;
    GLint                          mFrameLoc;
    GLint                          mTimeLoc;
    GLint                          mResolutionLoc;
    GLint                          mCameraOriginLoc;
    GLint                          mCameraTargetLoc;
    GLint                          mFFTLowLoc;
    GLint                          mFFTMidLoc;
    GLint                          mFFTHighLoc;
    std::unique_ptr<RenderTarget>  mRenderTarget;

    MainWindow&       mWindow;
    Visualizer const& mVisualizer;
    Renderer&         mRenderer;
    FileWatcher&      mFileWatcher;
    MainEditor&       mEditor;
    Camera&           mCamera;
    UniformsMenu&     mUniformsMenu;
};

}  // namespace ht