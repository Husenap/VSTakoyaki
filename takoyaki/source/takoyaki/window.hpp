#pragma once

#include <atomic>
#include <optional>
#include <thread>

#include <GLFW/glfw3.h>
#include <public.sdk/source/common/pluginview.h>
#include <public.sdk/source/vst/vsteditcontroller.h>

#include "takoyakicontroller.h"

namespace ht {
using namespace Steinberg;
using namespace Steinberg::Vst;

class Window : public EditorView {
public:
    Window(TakoyakiController* controller);
    ~Window();

    tresult PLUGIN_API isPlatformTypeSupported(FIDString type) SMTG_OVERRIDE;
    tresult PLUGIN_API attached(void* parent, FIDString type) SMTG_OVERRIDE;
    tresult PLUGIN_API removed() SMTG_OVERRIDE;

protected:
private:
    void open(void* parent);
    void close();
    void mainLoop();

    GLFWwindow*                   mWindow{};
    std::unique_ptr<std::jthread> mThread{};

    static inline std::atomic<Window*> mOwner{nullptr};

    TakoyakiController* mController{};
};

}  // namespace ht