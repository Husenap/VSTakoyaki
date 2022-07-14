#pragma once

#include "editor/widgets/camera.hpp"
#include "takoyakicontroller.h"
#include "util/file_watcher.hpp"

namespace ht {

class Visualizer {
public:
    Visualizer(TakoyakiController* controller);

    void Update();

private:
    TakoyakiController* mController;

    FileWatcher mFileWatcher;

    Buffer mRawData{};
    Buffer mFFT{};

    static inline float decayRate = 0.8f;
    static inline float fftLow    = 0.f;
    static inline float fftMid    = 0.f;
    static inline float fftHigh   = 0.f;

    Camera mCamera;
};

}  // namespace ht