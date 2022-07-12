#include "window.hpp"

#include <cmath>
#include <filesystem>
#include <iostream>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <imgui-knobs/imgui-knobs.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_opengl3_loader.h>
#include <imgui/imgui.h>
#include <public.sdk/source/main/moduleinit.h>

namespace ht {

static const auto IniFilePath =
    std::filesystem::temp_directory_path() / "VSTakoyaki" / "imgui.ini";
static const auto IniFileString = IniFilePath.generic_string();

Window::Window(TakoyakiController* controller)
    : EditorView(controller)
    , mController(controller) {
		
	wchar_t path[2048];
	if (GetModuleFileNameW(Steinberg::getPlatformModuleHandle(), path, 2048) > 0) {
        mResourcePath = path;
        mResourcePath = mResourcePath.parent_path().parent_path() / "Resources";
	}
}
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

    if (!std::filesystem::exists(IniFilePath)) {
        std::filesystem::create_directories(IniFilePath.parent_path());
        const auto imguiTemplatePath = mResourcePath / "imgui.ini";
        if (std::filesystem::exists(imguiTemplatePath))
            std::filesystem::copy(imguiTemplatePath, IniFilePath);
    }
    io.IniFilename = IniFileString.c_str();

    ImGuiStyle& style                     = ImGui::GetStyle();
    ImVec4*     colors                    = style.Colors;
    colors[ImGuiCol_Text]                 = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]         = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_ChildBg]              = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_WindowBg]             = ImVec4(0.25f, 0.25f, 0.25f, 0.75f);
    colors[ImGuiCol_PopupBg]              = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_Border]               = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
    colors[ImGuiCol_BorderShadow]         = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_FrameBg]              = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
    colors[ImGuiCol_FrameBgActive]        = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
    colors[ImGuiCol_TitleBg]              = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_TitleBgActive]        = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
    colors[ImGuiCol_MenuBarBg]            = ImVec4(0.33f, 0.33f, 0.33f, 0.75f);
    colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    colors[ImGuiCol_CheckMark]            = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
    colors[ImGuiCol_SliderGrab]           = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
    colors[ImGuiCol_Button]               = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
    colors[ImGuiCol_ButtonHovered]        = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
    colors[ImGuiCol_ButtonActive]         = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    colors[ImGuiCol_Header]               = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_HeaderHovered]        = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
    colors[ImGuiCol_HeaderActive]         = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
    colors[ImGuiCol_Separator]         = ImVec4(0.000f, 0.000f, 0.000f, 0.137f);
    colors[ImGuiCol_SeparatorHovered]  = ImVec4(0.700f, 0.671f, 0.600f, 0.290f);
    colors[ImGuiCol_SeparatorActive]   = ImVec4(0.702f, 0.671f, 0.600f, 0.674f);
    colors[ImGuiCol_ResizeGrip]        = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_ResizeGripActive]  = ImVec4(0.86f, 0.66f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLines]         = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]  = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]     = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
    colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_Tab]                   = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_TabHovered]            = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_TabActive]             = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    colors[ImGuiCol_TabUnfocused]          = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    colors[ImGuiCol_DockingPreview]        = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);

    style.PopupRounding = 3.f;

    style.WindowPadding    = ImVec2(4.f, 4.f);
    style.FramePadding     = ImVec2(6.f, 4.f);
    style.ItemSpacing      = ImVec2(3.f, 3.f);
    style.ItemInnerSpacing = ImVec2(3.f, 3.f);

    style.ScrollbarSize = 18.f;

    style.WindowBorderSize = 1.f;
    style.ChildBorderSize  = 1.f;
    style.PopupBorderSize  = 1.f;
    style.FrameBorderSize  = 0.f;

    style.WindowRounding    = 2.f;
    style.ChildRounding     = 2.f;
    style.FrameRounding     = 2.f;
    style.ScrollbarRounding = 2.f;
    style.GrabRounding      = 3.f;

    style.TabBorderSize = 0.f;
    style.TabRounding   = 3.f;

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

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport();
        ImGui::ShowDemoWindow();

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
            ImGuiKnobs::Knob("Decay Rate", &decayRate, 0.f, 1.f);

            ImGui::VSliderFloat(
                "##low", ImVec2(50, 150), &fftLow, 0.0f, 1.0f, "");
            ImGui::SameLine();
            ImGui::VSliderFloat(
                "##mid", ImVec2(50, 150), &fftMid, 0.0f, 1.0f, "");
            ImGui::SameLine();
            ImGui::VSliderFloat(
                "##high", ImVec2(50, 150), &fftHigh, 0.0f, 1.0f, "");

            ImGui::LabelText(
                "Resource Path", "%s", mResourcePath.generic_string().c_str());
        }
        ImGui::End();

        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(mWindow);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

}  // namespace ht