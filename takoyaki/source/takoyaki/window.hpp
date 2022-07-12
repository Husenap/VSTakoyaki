#pragma once

#include <atomic>
#include <optional>
#include <thread>

#include <GLFW/glfw3.h>
#include <public.sdk/source/common/pluginview.h>
#include <public.sdk/source/vst/vsteditcontroller.h>

#include "data_provider.hpp"
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

    tresult PLUGIN_API onWheel(float /*distance*/) SMTG_OVERRIDE {
        return kResultFalse;
    }
    tresult PLUGIN_API onKeyDown(char16 /*key*/,
                                 int16 /*keyMsg*/,
                                 int16 /*modifiers*/) SMTG_OVERRIDE {
        return kResultFalse;
    }
    tresult PLUGIN_API onKeyUp(char16 /*key*/,
                               int16 /*keyMsg*/,
                               int16 /*modifiers*/) SMTG_OVERRIDE {
        return kResultFalse;
    }

    // tresult PLUGIN_API getSize(ViewRect* size) SMTG_OVERRIDE;
    tresult PLUGIN_API onSize(ViewRect* newSize) SMTG_OVERRIDE;

    tresult PLUGIN_API onFocus(TBool /*state*/) SMTG_OVERRIDE {
        return kResultFalse;
    }
    tresult PLUGIN_API setFrame(IPlugFrame* frame) SMTG_OVERRIDE {
        plugFrame = frame;
        return kResultTrue;
    }

    tresult PLUGIN_API canResize() SMTG_OVERRIDE { return kResultTrue; }
    tresult PLUGIN_API checkSizeConstraint(ViewRect* /*rect*/) SMTG_OVERRIDE {
        return kResultFalse;
    }

protected:
private:
    void open(void* parent);
    void close();
    void mainLoop();

    GLFWwindow*                   mWindow{};
    std::unique_ptr<std::jthread> mThread{};

    static inline std::atomic<Window*> mOwner{nullptr};

    static constexpr int Width  = 1280;
    static constexpr int Height = 720;

    TakoyakiController* mController{};

    Buffer mRawData{};
    Buffer mFFT{};

    DataProvider<std::pair<int, int>> mSize;
};

}  // namespace ht