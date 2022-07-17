#include "service_manager.hpp"

#include "main_editor.hpp"
#include "takoyaki.hpp"
#include "takoyaki/visualizer/graphics/main_window.hpp"
#include "takoyaki/visualizer/graphics/renderer.hpp"
#include "widgets/camera.hpp"
#include "widgets/preview.hpp"
#include "widgets/uniforms_menu.hpp"

namespace ht {

ServiceManager::ServiceManager(MainWindow&       window,
                               Visualizer const& visualizer) {
    mCamera       = std::make_unique<Camera>();
    mPreview      = std::make_unique<Preview>();
    mUniformsMenu = std::make_unique<UniformsMenu>();
    mEditor = std::make_unique<MainEditor>(*mCamera, *mPreview, *mUniformsMenu);
    mFileWatcher = std::make_unique<FileWatcher>();
    mRenderer    = std::make_unique<Renderer>();

    mTakoyaki = std::make_unique<Takoyaki>(window,
                                           visualizer,
                                           *mRenderer,
                                           *mFileWatcher,
                                           *mEditor,
                                           *mCamera,
                                           *mUniformsMenu);
}
ServiceManager::~ServiceManager() {}

void ServiceManager::Tick() {
    mTakoyaki->Tick();
}

}  // namespace ht