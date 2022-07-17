#include "visualizer.hpp"

#include <imgui-knobs/imgui-knobs.h>
#include <imgui/imgui.h>

namespace ht {
Visualizer::Visualizer(TakoyakiController* controller)
    : mController(controller) {}

void Visualizer::Update() {
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

        fftLow *= amplifier;
        fftMid *= amplifier;
        fftHigh *= amplifier;

        mController->mData.request();
    }

    if (ImGui::Begin("FFT")) {
        {
            static float lowerbound = -1.f;
            static float upperbound = 1.f;
            ImGui::DragFloatRange2("Bound##RawData", &lowerbound, &upperbound);

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
        ImGui::SameLine();
        ImGuiKnobs::Knob("Amp", &amplifier, 0.f, 10.f);

        ImGui::VSliderFloat("##low", ImVec2(50, 150), &fftLow, 0.0f, 1.0f, "");
        ImGui::SameLine();
        ImGui::VSliderFloat("##mid", ImVec2(50, 150), &fftMid, 0.0f, 1.0f, "");
        ImGui::SameLine();
        ImGui::VSliderFloat(
            "##high", ImVec2(50, 150), &fftHigh, 0.0f, 1.0f, "");
    }
    ImGui::End();
}

}  // namespace ht
