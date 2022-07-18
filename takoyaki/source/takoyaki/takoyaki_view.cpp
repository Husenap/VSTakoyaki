#include "takoyaki_view.hpp"

#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>
#include <tuple>

#include <glad/gl.h>

#include "imgui_wrapper.hpp"
#include "visualizer/editor/service_manager.hpp"
#include "visualizer/visualizer.hpp"

namespace ht {

TakoyakiView::TakoyakiView(TakoyakiController* controller)
    : EditorView(controller)
    , mController(controller) {}

TakoyakiView::~TakoyakiView() {}

static auto translateKey(char16 key, int16 keyMsg, int16 modifiers) {
    if (key == 0) {
        if (keyMsg >= KEY_F1 && keyMsg <= KEY_F12) {
            key = GLFW_KEY_F1 + (keyMsg - KEY_F1);
        } else {
			key = VirtualKeyCodeToChar(static_cast<uint8>(keyMsg));
        }
    } else if (key >= 'a' && key <= 'z') {
        key -= 'a' - 'A';
    }

    int mods = 0;
    if (modifiers & kShiftKey) mods |= GLFW_MOD_SHIFT;
    if (modifiers & kAlternateKey) mods |= GLFW_MOD_ALT;
    if (modifiers & kCommandKey) mods |= GLFW_MOD_CONTROL;

    return std::make_tuple(key, keyMsg, mods);
}

tresult TakoyakiView::onKeyDown(char16 key, int16 keyMsg, int16 modifiers) {
    if (mMainWindow) {
        const auto [keyCode, scanCode, mods] =
            translateKey(key, keyMsg, modifiers);
        mMainWindow->WindowInputCallback(
            mMainWindow->GetHandle(), keyCode, scanCode, GLFW_PRESS, mods);
        return kResultTrue;
    }
    return kResultFalse;
}

tresult TakoyakiView::onKeyUp(char16 key, int16 keyMsg, int16 modifiers) {
    if (mMainWindow) {
        const auto [keyCode, scanCode, mods] =
            translateKey(key, keyMsg, modifiers);
        mMainWindow->WindowInputCallback(
            mMainWindow->GetHandle(), keyCode, scanCode, GLFW_RELEASE, mods);
        return kResultTrue;
    }
    return kResultFalse;
}

tresult TakoyakiView::isPlatformTypeSupported(FIDString type) {
    if (strcmp(type, kPlatformTypeHWND) == 0) {
        return kResultTrue;
    }
    return kInvalidArgument;
}

tresult TakoyakiView::attached(void* parent, FIDString type) {
    open(parent);
    return EditorView::attached(parent, type);
}
tresult TakoyakiView::removed() {
    close();
    return EditorView::removed();
}

void TakoyakiView::open(void* parent) {
    if (mOwner != nullptr) return;
    mOwner = this;

    mMainWindow = std::make_unique<MainWindow>(1600, 900, "VSTakoyaki", parent);

    setRect({0, 0, 1600, 900});

    mThread = std::make_unique<std::jthread>([this]() { mainLoop(); });
}

void TakoyakiView::close() {
    if (mOwner != this) return;
    mThread->request_stop();
    mThread->join();
    mThread.reset();

    mMainWindow.reset();
    mOwner = nullptr;
}

void TakoyakiView::mainLoop() {
    glfwMakeContextCurrent(mMainWindow->GetHandle());
    gladLoadGL(static_cast<GLADloadfunc>(glfwGetProcAddress));
    glfwSwapInterval(1);

    ImGuiWrapper::Initialize(mMainWindow->GetHandle());

    auto visualizer = std::make_unique<Visualizer>(mController);

    ServiceManager serviceManager(*mMainWindow, *visualizer);

    while (!mThread->get_stop_token().stop_requested()) {
        mMainWindow->PollEvents();

        ImGuiWrapper::BeginFrame();

        visualizer->Update();
        serviceManager.Tick();

        ImGuiWrapper::EndFrame();

        mMainWindow->SwapBuffers();
    }

    ImGuiWrapper::Destroy();
}

}  // namespace ht