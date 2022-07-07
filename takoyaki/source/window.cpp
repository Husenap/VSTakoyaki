#include "window.hpp"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <imgui-knobs/imgui-knobs.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_opengl3_loader.h>
#include <imgui/imgui.h>

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

tresult PLUGIN_API Window::onSize(ViewRect* newSize) {
    return EditorView::onSize(newSize);
}

void PLUGIN_API Window::open(void* parent) {
    if (mOwner != nullptr) return;
    mOwner = this;
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    mWindow = glfwCreateWindow(Width, Height, "VSTakoyaki", NULL, NULL);

    HWND hWnd = glfwGetWin32Window(mWindow);

    SetParent(hWnd, static_cast<HWND>(parent));

    const LONG nNewStyle =
        (GetWindowLong(hWnd, GWL_STYLE) & ~WS_POPUP) | WS_CHILDWINDOW;
    SetWindowLong(hWnd, GWL_STYLE, nNewStyle);
    ShowWindow(hWnd, SW_SHOW);

    setRect({0, 0, Width, Height});

    mThread = std::make_unique<std::jthread>([this]() { mainLoop(); });
}

void PLUGIN_API Window::close() {
    if (mOwner != this) return;
    mThread->request_stop();
    mThread->join();
    mThread.reset();

    glfwDestroyWindow(mWindow);
    glfwTerminate();
    mOwner = nullptr;
}

void PLUGIN_API Window::mainLoop() {
    glfwMakeContextCurrent(mWindow);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    while (!mThread->get_stop_token().stop_requested()) {
        static float decayRate = 0.9f;
        if (mController->mHasNewData) {
            if (mData.size() != mController->mData.size()) {
                mData = mController->mData;
            } else {
                for (std::size_t i = 0; i < mData.size(); ++i) {
                    mData[i] = mData[i] * decayRate +
                               mController->mData[i] * (1.f - decayRate);
                }
            }
            mController->mHasNewData = false;
        }

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport();
        ImGui::ShowDemoWindow();

        ImGuiKnobs::Knob("Decay Rate", &decayRate, 0.f, 1.f);
        ImGui::SameLine();
        static float lowerbound = 0.f;
        ImGuiKnobs::Knob("Lower Bound", &lowerbound, 0.f, 128.f);
        ImGui::SameLine();
        static float upperbound = 64.f;
        ImGuiKnobs::Knob("Upper Bound", &upperbound, 0.f, 128.f);
        ImGui::PlotHistogram("Histogram",
                             mData.data(),
                             mData.size() / 8,
                             0,
                             NULL,
                             lowerbound,
                             upperbound,
                             ImVec2(0, 150.0f));

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(mWindow);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

}  // namespace ht