#pragma once

#include <functional>
#include <stdexcept>
#include <vector>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>

#include "takoyaki/visualizer/util/input.hpp"

namespace ht {

template <typename DERIVED_TYPE>
class BaseWindow {
public:
    using KeyInputCallbackType        = std::function<void(const KeyInput&)>;
    using MouseInputCallbackType      = std::function<void(const MouseInput&)>;
    using CursorInputCallbackType     = std::function<void(const CursorInput&)>;
    using FramebufferSizeCallbackType = std::function<void(const glm::ivec2&)>;

public:
    BaseWindow(int         width,
               int         height,
               const char* title,
               void*       parent = nullptr) {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW!");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        if (parent) {
            glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        }

        mWindow = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!mWindow) {
            throw std::runtime_error("Failed to create a GLFW window!");
        }

        if (parent) {
            HWND hWnd = glfwGetWin32Window(mWindow);

            SetParent(hWnd, static_cast<HWND>(parent));

            const LONG nNewStyle =
                (GetWindowLong(hWnd, GWL_STYLE) & ~WS_POPUP) | WS_CHILDWINDOW;
            SetWindowLong(hWnd, GWL_STYLE, nNewStyle);
            ShowWindow(hWnd, SW_SHOW);
        }

        glfwSetKeyCallback(mWindow, WindowInputCallback);
        glfwSetFramebufferSizeCallback(mWindow, WindowFramebufferSizeCallback);
        glfwSetCursorPosCallback(mWindow, WindowCursorPosCallback);
        glfwSetMouseButtonCallback(mWindow, WindowMouseButtonCallback);

        glfwGetFramebufferSize(
            mWindow, &mFramebufferSize.x, &mFramebufferSize.y);

        double mx, my;
        glfwGetCursorPos(mWindow, &mx, &my);
        mMousePos   = glm::vec2((float)mx, (float)my);
        mMouseDelta = glm::vec2(0.0f, 0.0f);
    }
    virtual ~BaseWindow() {
        glfwDestroyWindow(mWindow);
        glfwTerminate();
    }

    static void WindowInputCallback(
        GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key < 0 || key > GLFW_KEY_LAST) return;

        auto userWindow =
            reinterpret_cast<DERIVED_TYPE*>(glfwGetWindowUserPointer(window));
        if (userWindow) {
            KeyInput input{key, scancode, action, mods};
            for (auto callback : userWindow->mKeyInputListeners)
                callback(input);
        } else {
            throw std::runtime_error("Failed to process window input callback");
        }
    }

    static void WindowCursorPosCallback(GLFWwindow* window,
                                        double      x,
                                        double      y) {
        auto userWindow =
            reinterpret_cast<DERIVED_TYPE*>(glfwGetWindowUserPointer(window));
        if (userWindow) {
            glm::vec2 newPos((float)x, (float)y);
            userWindow->mMouseDelta = newPos - userWindow->mMousePos;
            userWindow->mMousePos   = newPos;
            CursorInput input;
            input.pos   = userWindow->mMousePos;
            input.delta = userWindow->mMouseDelta;
            if (userWindow->mSkipMouseInput > 0) {
                --userWindow->mSkipMouseInput;
            } else {
                for (auto callback : userWindow->mCursorInputListeners)
                    callback(input);
            }
        } else {
            throw std::runtime_error(
                "Failed to process window cursor callback");
        }
    }

    static void WindowMouseButtonCallback(GLFWwindow* window,
                                          int         button,
                                          int         action,
                                          int         mods) {
        auto userWindow =
            reinterpret_cast<DERIVED_TYPE*>(glfwGetWindowUserPointer(window));
        if (userWindow) {
            MouseInput input{button, action, mods};
            for (auto callback : userWindow->mMouseInputListeners)
                callback(input);
        } else {
            throw std::runtime_error(
                "Failed to process window mouse button callback");
        }
    }

    static void WindowFramebufferSizeCallback(GLFWwindow* window,
                                              int         width,
                                              int         height) {
        auto userWindow =
            static_cast<DERIVED_TYPE*>(glfwGetWindowUserPointer(window));
        if (userWindow) {
            glm::ivec2 size{width, height};
            userWindow->mFramebufferSize = size;
            for (auto callback : userWindow->mFramebufferSizeListeners)
                callback(size);
        } else {
            throw std::runtime_error(
                "Failed to process window framebuffer size callback");
        }
    }

public:
    void AddInputListener(KeyInputCallbackType callback) {
        if (callback) mKeyInputListeners.emplace_back(callback);
    }
    void AddInputListener(MouseInputCallbackType callback) {
        if (callback) mMouseInputListeners.emplace_back(callback);
    }
    void AddInputListener(CursorInputCallbackType callback) {
        if (callback) mCursorInputListeners.emplace_back(callback);
    }
    void AddFramebufferSizeListener(FramebufferSizeCallbackType callback) {
        if (callback) {
            mFramebufferSizeListeners.emplace_back(callback);
            callback(mFramebufferSize);
        }
    }

    void PollEvents() { glfwPollEvents(); }
    void SwapBuffers() { glfwSwapBuffers(mWindow); }

    GLFWwindow* GetHandle() { return mWindow; }

    void SetInputMode(int mode, int value) {
        glfwSetInputMode(mWindow, mode, value);
        mSkipMouseInput = 2;
    }

    void RequestClose() { glfwSetWindowShouldClose(mWindow, GLFW_TRUE); }
    bool ShouldClose() const { return glfwWindowShouldClose(mWindow); }

    const glm::ivec2& GetFramebufferSize() const { return mFramebufferSize; }

protected:
    GLFWwindow* mWindow;
    glm::ivec2  mFramebufferSize;
    glm::vec2   mMousePos;
    glm::vec2   mMouseDelta;
    int         mSkipMouseInput = 0;

private:
    std::vector<KeyInputCallbackType>        mKeyInputListeners;
    std::vector<MouseInputCallbackType>      mMouseInputListeners;
    std::vector<CursorInputCallbackType>     mCursorInputListeners;
    std::vector<FramebufferSizeCallbackType> mFramebufferSizeListeners;
};

}  // namespace ht