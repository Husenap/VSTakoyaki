#pragma once

namespace ht {
class Renderer;
class FileWatcher;
class MainWindow;
class MainEditor;
class Takoyaki;
class Camera;
class UniformsMenu;
class Preview;
class Visualizer;

class ServiceManager {
public:
    ServiceManager(MainWindow& window, Visualizer const& visualizer);
    ~ServiceManager();

    void Tick();

private:
    std::unique_ptr<MainEditor>   mEditor;
    std::unique_ptr<FileWatcher>  mFileWatcher;
    std::unique_ptr<Renderer>     mRenderer;
    std::unique_ptr<Camera>       mCamera;
    std::unique_ptr<Preview>      mPreview;
    std::unique_ptr<UniformsMenu> mUniformsMenu;

    std::unique_ptr<Takoyaki> mTakoyaki;
};

}  // namespace ht