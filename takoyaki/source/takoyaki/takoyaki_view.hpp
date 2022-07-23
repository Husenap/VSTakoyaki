#pragma once

#include <atomic>
#include <optional>
#include <thread>

#include <pluginterfaces/base/keycodes.h>
#include <public.sdk/source/common/pluginview.h>
#include <public.sdk/source/vst/vsteditcontroller.h>

#include "takoyakicontroller.h"
#include "visualizer/graphics/main_window.hpp"

namespace ht {
using namespace Steinberg;
using namespace Steinberg::Vst;

class TakoyakiView : public EditorView {
public:
    TakoyakiView(TakoyakiController* controller);
    ~TakoyakiView();

    tresult isPlatformTypeSupported(FIDString type) override;
    tresult attached(void* parent, FIDString type) override;
    tresult removed() override;

    tresult onKeyDown(char16 key, int16 keyMsg, int16 modifiers) override;
    tresult onKeyUp(char16 key, int16 keyMsg, int16 modifiers) override;

protected:
private:
    void open(void* parent);
    void close();
    void mainLoop();

    std::unique_ptr<std::jthread> mThread{};

    static inline std::atomic<TakoyakiView*> mOwner{nullptr};

    TakoyakiController* mController{};

    std::unique_ptr<MainWindow> mMainWindow{};
};

}  // namespace ht