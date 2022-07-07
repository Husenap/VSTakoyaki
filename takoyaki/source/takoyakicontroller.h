//------------------------------------------------------------------------
// Copyright(c) 2022 Hussein Taher.
//------------------------------------------------------------------------

#pragma once

#include <atomic>
#include <vector>

#include "public.sdk/source/vst/vsteditcontroller.h"

namespace ht {

//------------------------------------------------------------------------
//  TakoyakiController
//------------------------------------------------------------------------
class TakoyakiController : public Steinberg::Vst::EditControllerEx1 {
public:
    //------------------------------------------------------------------------
    TakoyakiController()                = default;
    ~TakoyakiController() SMTG_OVERRIDE = default;

    // Create function
    static Steinberg::FUnknown* createInstance(void* /*context*/) {
        return (Steinberg::Vst::IEditController*)new TakoyakiController;
    }

    // IPluginBase
    Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown* context)
        SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API terminate() SMTG_OVERRIDE;

    // EditController
    Steinberg::tresult PLUGIN_API setComponentState(Steinberg::IBStream* state)
        SMTG_OVERRIDE;
    Steinberg::IPlugView* PLUGIN_API createView(Steinberg::FIDString name)
        SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API setState(Steinberg::IBStream* state)
        SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API getState(Steinberg::IBStream* state)
        SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API
    setParamNormalized(Steinberg::Vst::ParamID    tag,
                       Steinberg::Vst::ParamValue value) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API
    getParamStringByValue(Steinberg::Vst::ParamID    tag,
                          Steinberg::Vst::ParamValue valueNormalized,
                          Steinberg::Vst::String128  string) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API getParamValueByString(
        Steinberg::Vst::ParamID     tag,
        Steinberg::Vst::TChar*      string,
        Steinberg::Vst::ParamValue& valueNormalized) SMTG_OVERRIDE;

    Steinberg::tresult PLUGIN_API notify(Steinberg::Vst::IMessage* message);

    //---Interface---------
    DEFINE_INTERFACES
    // Here you can add more supported VST3 interfaces
    // DEF_INTERFACE (Vst::IXXX)
    END_DEFINE_INTERFACES(EditController)
    DELEGATE_REFCOUNT(EditController)

    //------------------------------------------------------------------------
    std::vector<float> mData;
    std::atomic_bool   mHasNewData{false};

protected:
};

//------------------------------------------------------------------------
}  // namespace ht
