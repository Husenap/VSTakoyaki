#include "window.hpp"

#include <cmath>
#include <filesystem>
#include <iostream>

/*
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
*/

#include "imgui_wrapper.hpp"
#include "visualizer/visualizer.hpp"

namespace ht {

Window::Window(TakoyakiController* controller)
    : EditorView(controller)
    , mController(controller) {}
Window::~Window() {}

tresult PLUGIN_API Window::isPlatformTypeSupported(FIDString type) {
    if (strcmp(type, kPlatformTypeHWND) == 0) {
        return kResultTrue;
    }
    return kInvalidArgument;
}

tresult PLUGIN_API Window::attached(void* parent, FIDString type) {
    open(parent);
    return EditorView::attached(parent, type);
}
tresult PLUGIN_API Window::removed() {
    close();
    return EditorView::removed();
}

void Window::open(void* parent) {
    if (mOwner != nullptr) return;
    mOwner = this;
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /*
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    */

    mWindow = glfwCreateWindow(1600, 900, "VSTakoyaki", NULL, NULL);

    /*
    HWND hWnd = glfwGetWin32Window(mWindow);

    SetParent(hWnd, static_cast<HWND>(parent));

    const LONG nNewStyle =
        (GetWindowLong(hWnd, GWL_STYLE) & ~WS_POPUP) | WS_CHILDWINDOW;
    SetWindowLong(hWnd, GWL_STYLE, nNewStyle);
    ShowWindow(hWnd, SW_SHOW);
    */

    mThread = std::make_unique<std::jthread>([this]() { mainLoop(); });
}

void Window::close() {
    if (mOwner != this) return;
    mThread->request_stop();
    mThread->join();
    mThread.reset();

    glfwDestroyWindow(mWindow);
    glfwTerminate();
    mOwner = nullptr;
}

void Window::mainLoop() {
    glfwMakeContextCurrent(mWindow);
    glfwSwapInterval(1);

    ImGuiWrapper::Initialize(mWindow);

    Visualizer visualizer{mController};

    while (!mThread->get_stop_token().stop_requested()) {
        glfwPollEvents();

        ImGuiWrapper::BeginFrame();

        visualizer.Update();

        ImGuiWrapper::EndFrame();

        glfwSwapBuffers(mWindow);
    }

    ImGuiWrapper::Destroy();
}

}  // namespace ht