//------------------------------------------------------------------------
// Copyright(c) 2022 Hussein Taher.
//------------------------------------------------------------------------

#include "takoyakicontroller.h"

#include "fft.hpp"
#include "takoyakicids.h"
#include "window.hpp"

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

template <typename T>
int popcount(T t) {
    int count = 0;
    while (t) {
        ++count;
        t ^= (t & -t);
    }
    return count;
}
//------------------------------------------------------------------------
tresult PLUGIN_API TakoyakiController::notify(Vst::IMessage* message) {
    if (!message) {
        return kInvalidArgument;
    }

    if (!mHasNewData && strcmp(message->getMessageID(), "Data") == 0) {
        const void* data = nullptr;
        uint32      size = 0;
        if (message->getAttributes()->getBinary("Data", data, size) ==
            kResultOk) {
            mData.resize(size / sizeof(float));
            memcpy(mData.data(), data, size);

            if (popcount(mData.size()) == 1) {
                std::vector<std::complex<float>> A(mData.begin(), mData.end());
                ht::FFT(A);
                for (int i = 0; i < mData.size(); ++i) {
                    mData[i] = std::sqrt(A[i].real() * A[i].real() +
                                         A[i].imag() * A[i].imag());
                }
            }

            mHasNewData = true;
        }
    }

    return kResultOk;
}
}  // namespace ht
