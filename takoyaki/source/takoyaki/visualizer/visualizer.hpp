#pragma once

#include "takoyakicontroller.h"

namespace ht {

class Visualizer {
public:
    Visualizer(TakoyakiController* controller);

    void Update();

    float getFFTLow() const { return fftLow; }
    float getFFTMid() const { return fftMid; }
    float getFFTHigh() const { return fftHigh; }

private:
    TakoyakiController* mController;

    Buffer mRawData{};
    Buffer mFFT{};

    static inline float decayRate   = 0.8f;
    static inline float sensitivity = 1.f;
    static inline float fftLow      = 0.f;
    static inline float fftMid      = 0.f;
    static inline float fftHigh     = 0.f;
};

}  // namespace ht