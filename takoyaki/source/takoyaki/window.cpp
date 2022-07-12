#include "window.hpp"

#include <cmath>
#include <filesystem>
#include <iostream>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <imgui-knobs/imgui-knobs.h>
#include <imgui/imgui.h>

#include "imgui_wrapper.hpp"

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
    if (newSize) {
        auto& [width, height] = *mSize;

        width  = std::max<int>(1, newSize->getWidth());
        height = std::max<int>(1, newSize->getHeight());

        mSize.serve();
    }
    return EditorView::onSize(newSize);
}

void Window::open(void* parent) {
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

    while (!mThread->get_stop_token().stop_requested()) {
        static float decayRate = 0.8f;
        static float fftLow    = 0.f;
        static float fftMid    = 0.f;
        static float fftHigh   = 0.f;

        if (mController->mData) {
            const auto& [rawData, fftData] = *mController->mData;

            mRawData = rawData;

            fftLow  = 0.f;
            fftMid  = 0.f;
            fftHigh = 0.f;

            if (std::isnan(mFFT[0])) {
                mFFT = fftData;
            } else {
                for (std::size_t i = 0; i < mFFT.size(); ++i) {
                    float amplitude = fftData[i];

                    if (amplitude > mFFT[i]) {
                        mFFT[i] = amplitude;
                    } else {
                        mFFT[i] =
                            mFFT[i] * decayRate + amplitude * (1.f - decayRate);
                    }

                    amplitude = mFFT[i];

                    if (i < BufferSize / 512) {
                        fftLow = std::max(fftLow, amplitude);
                    } else if (i < BufferSize / 128) {
                        fftMid = std::max(fftMid, amplitude);
                    } else if (i < BufferSize / 4) {
                        fftHigh = std::max(fftHigh, amplitude);
                    }
                }
            }

            mController->mData.request();
        }

        if (mSize) {
            const auto [width, height] = *mSize;
            mSize.request();
            glfwSetWindowSize(mWindow, width, height);
        }

        glfwPollEvents();

        ImGuiWrapper::BeginFrame();

        if (ImGui::Begin("FFT")) {
            {
                static float lowerbound = -1.f;
                static float upperbound = 1.f;
                ImGui::DragFloatRange2(
                    "Bound##RawData", &lowerbound, &upperbound);

                ImGui::PlotHistogram("Raw Data",
                                     mRawData.data(),
                                     mRawData.size(),
                                     0,
                                     NULL,
                                     lowerbound,
                                     upperbound,
                                     ImVec2(0, 150.0f));
            }

            {
                static float lowerbound = 0.f;
                static float upperbound = 4.f;
                ImGui::DragFloatRange2("Bound##FFT", &lowerbound, &upperbound);
                ImGui::PlotHistogram("FFT",
                                     mFFT.data(),
                                     mFFT.size() / 16,
                                     0,
                                     NULL,
                                     lowerbound,
                                     upperbound,
                                     ImVec2(0, 150.0f));
            }
            ImGuiKnobs::Knob("Decay", &decayRate, 0.f, 1.f);

            ImGui::VSliderFloat(
                "##low", ImVec2(50, 150), &fftLow, 0.0f, 1.0f, "");
            ImGui::SameLine();
            ImGui::VSliderFloat(
                "##mid", ImVec2(50, 150), &fftMid, 0.0f, 1.0f, "");
            ImGui::SameLine();
            ImGui::VSliderFloat(
                "##high", ImVec2(50, 150), &fftHigh, 0.0f, 1.0f, "");

            ImGui::LabelText("Resource Path",
                             "%s",
                             TakoyakiController::getResourcesPath()
                                 .generic_string()
                                 .c_str());
        }
        ImGui::End();

        ImGuiWrapper::EndFrame();

        glfwSwapBuffers(mWindow);
    }

    ImGuiWrapper::Destroy();
}

}  // namespace ht