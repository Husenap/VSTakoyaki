#include "takoyaki_view.hpp"

#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>

#include <glad/gl.h>

#include "imgui_wrapper.hpp"
#include "visualizer/visualizer.hpp"

namespace ht {

TakoyakiView::TakoyakiView(TakoyakiController* controller)
    : EditorView(controller)
    , mController(controller) {}

TakoyakiView::~TakoyakiView() {}

tresult PLUGIN_API TakoyakiView::isPlatformTypeSupported(FIDString type) {
    if (strcmp(type, kPlatformTypeHWND) == 0) {
        return kResultTrue;
    }
    return kInvalidArgument;
}

tresult PLUGIN_API TakoyakiView::attached(void* parent, FIDString type) {
    open(parent);
    return EditorView::attached(parent, type);
}
tresult PLUGIN_API TakoyakiView::removed() {
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

    while (!mThread->get_stop_token().stop_requested()) {
        mMainWindow->PollEvents();

        ImGuiWrapper::BeginFrame();

        visualizer->Update();

        ImGuiWrapper::EndFrame();

        mMainWindow->SwapBuffers();
    }

    ImGuiWrapper::Destroy();
}

}  // namespace ht