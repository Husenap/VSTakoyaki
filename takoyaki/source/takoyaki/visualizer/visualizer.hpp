#pragma once

#include "takoyakicontroller.h"

namespace ht {

class Visualizer {
public:
    Visualizer(TakoyakiController* controller)
        : mController(controller) {}

    void Update();

private:
    TakoyakiController* mController;

    Buffer mRawData{};
    Buffer mFFT{};

    static inline float decayRate = 0.8f;
    static inline float fftLow    = 0.f;
    static inline float fftMid    = 0.f;
    static inline float fftHigh   = 0.f;
};

}  // namespace ht