#include "main_window.hpp"

namespace ht {

MainWindow::MainWindow(int width, int height, const char* title, void* parent)
    : BaseWindow(width, height, title, parent) {
    glfwSetWindowUserPointer(mWindow, this);
}

}  // namespace ht
