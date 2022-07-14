#pragma once

#include "base_window.hpp"

namespace ht {

class MainWindow : public BaseWindow<MainWindow> {
public:
    MainWindow(int         width,
               int         height,
               const char* title,
               void*       parent = nullptr);

private:
};

}  // namespace ht