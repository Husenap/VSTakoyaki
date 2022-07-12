//------------------------------------------------------------------------
// Copyright(c) 2022 Hussein Taher.
//------------------------------------------------------------------------

#include "takoyakicontroller.h"

#include <algorithm>

#include <public.sdk/source/main/moduleinit.h>
#include <windows.h>

#include "takoyaki/fft.hpp"
#include "takoyaki/window.hpp"
#include "takoyakicids.h"

using namespace Steinberg;

namespace ht {

//------------------------------------------------------------------------
// TakoyakiController Implementation
//------------------------------------------------------------------------
tresult PLUGIN_API TakoyakiController::initialize(FUnknown* context) {
    // Here the Plug-in will be instanciated

    //---do not forget to call parent ------
    tresult result = EditControllerEx1::initialize(context);
    if (result != kResultOk) {
        return result;
    }

    wchar_t path[2048];
    if (GetModuleFileNameW(Steinberg::getPlatformModuleHandle(), path, 2048) >
        0) {
        mResourcesPath =
            std::filesystem::path(path).parent_path().parent_path() /
            "Resources";
    }

    // Here you could register some parameters

    return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API TakoyakiController::terminate() {
    // Here the Plug-in will be de-instanciated, last possibility to remove some
    // memory!

    //---do not forget to call parent ------
    return EditControllerEx1::terminate();
}

//------------------------------------------------------------------------
tresult PLUGIN_API TakoyakiController::setComponentState(IBStream* state) {
    // Here you get the state of the component (Processor part)
    if (!state) return kResultFalse;

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API TakoyakiController::setState(IBStream* state) {
    // Here you get the state of the controller
    if (!state) return kResultFalse;

    return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API TakoyakiController::getState(IBStream* state) {
    // Here you are asked to deliver the state of the controller (if needed)
    // Note: the real state of your plug-in is saved in the processor
    if (!state) return kResultFalse;

    return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API TakoyakiController::createView(FIDString name) {
    // Here the Host wants to open your editor (if you have one)
    if (FIDStringsEqual(name, Vst::ViewType::kEditor)) {
        // create your editor here and return a IPlugView ptr of it
        return new Window(this);
    }
    return nullptr;
}

//------------------------------------------------------------------------
tresult PLUGIN_API TakoyakiController::setParamNormalized(
    Vst::ParamID tag, Vst::ParamValue value) {
    // called by host to update your parameters
    tresult result = EditControllerEx1::setParamNormalized(tag, value);
    return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API TakoyakiController::getParamStringByValue(
    Vst::ParamID tag, Vst::ParamValue valueNormalized, Vst::String128 string) {
    // called by host to get a string for given normalized value of a specific
    // parameter (without having to set the value!)
    return EditControllerEx1::getParamStringByValue(
        tag, valueNormalized, string);
}

//------------------------------------------------------------------------
tresult PLUGIN_API TakoyakiController::getParamValueByString(
    Vst::ParamID tag, Vst::TChar* string, Vst::ParamValue& valueNormalized) {
    // called by host to get a normalized value from a string representation of
    // a specific parameter (without having to set the value!)
    return EditControllerEx1::getParamValueByString(
        tag, string, valueNormalized);
}

//------------------------------------------------------------------------
tresult PLUGIN_API TakoyakiController::notify(Vst::IMessage* message) {
    if (!message) {
        return kInvalidArgument;
    }

    if (strcmp(message->getMessageID(), "Data") == 0) {
        const void* data  = nullptr;
        uint32      bytes = 0;

        if (message->getAttributes()->getBinary("Data", data, bytes) ==
            kResultOk) {
            const uint32 numSamples = bytes / sizeof(float);
            const float* samples    = static_cast<const float*>(data);

            for (uint32 i = 0; i < numSamples; ++i) {
                mCircularBuffer[mBufferIndex] = samples[i];
                mBufferIndex                  = (mBufferIndex + 1) % BufferSize;
            }
        }
    }

    if (!mData) {
        auto& [rawData, fftData] = *mData;

        memcpy(rawData.data(),
               mCircularBuffer.data() + mBufferIndex,
               (BufferSize - mBufferIndex) * sizeof(float));
        memcpy(rawData.data() + BufferSize - mBufferIndex,
               mCircularBuffer.data(),
               mBufferIndex * sizeof(float));

        std::vector<std::complex<float>> A(rawData.begin(), rawData.end());

        // Todo: Precalculate window
        // Todo: Add OpenMP
        {  // Hamming Window
            const float factor = 2 * 3.1415926535898f / (BufferSize - 1);
            for (int i = 0; i < BufferSize; ++i) {
                A[i] *= 0.53836f - 0.46164f * std::cosf(factor * i);
            }
        }

        ht::FFT(A);

        for (int i = 0; i < BufferSize; ++i) {
            A[i] *= 4.f / BufferSize;

            const auto re = A[i].real();
            const auto im = A[i].imag();

            fftData[i] = 20.f * std::log10f(1.f + re * re + im * im);
        }

        mData.serve();
    }

    return kResultOk;
}
}  // namespace ht
