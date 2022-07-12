//------------------------------------------------------------------------
// Copyright(c) 2022 Hussein Taher.
//------------------------------------------------------------------------

#include "takoyakiprocessor.h"

#include <string>

#include <base/source/fstreamer.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>
#include <public.sdk/source/vst/vstaudioprocessoralgo.h>

#include "takoyakicids.h"

using namespace Steinberg;

namespace ht {
//------------------------------------------------------------------------
// TakoyakiProcessor
//------------------------------------------------------------------------
TakoyakiProcessor::TakoyakiProcessor() {
    //--- set the wanted controller for our processor
    setControllerClass(kTakoyakiControllerUID);
}

//------------------------------------------------------------------------
TakoyakiProcessor::~TakoyakiProcessor() {}

//------------------------------------------------------------------------
tresult PLUGIN_API TakoyakiProcessor::initialize(FUnknown* context) {
    // Here the Plug-in will be instanciated

    //---always initialize the parent-------
    tresult result = AudioEffect::initialize(context);
    // if everything Ok, continue
    if (result != kResultOk) {
        return result;
    }

    //--- create Audio IO ------
    addAudioInput(STR16("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
    addAudioOutput(STR16("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);

    /* If you don't need an event bus, you can remove the next line */
    addEventInput(STR16("Event In"), 1);

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API TakoyakiProcessor::terminate() {
    // Here the Plug-in will be de-instanciated, last possibility to remove some
    // memory!

    //---do not forget to call parent ------
    return AudioEffect::terminate();
}

//------------------------------------------------------------------------
tresult PLUGIN_API TakoyakiProcessor::setActive(TBool state) {
    //--- called when the Plug-in is enable/disable (On/Off) -----
    return AudioEffect::setActive(state);
}

//------------------------------------------------------------------------
tresult PLUGIN_API TakoyakiProcessor::process(Vst::ProcessData& data) {
    //--- First : Read inputs parameter changes-----------

    /*if (data.inputParameterChanges)
    {
        int32 numParamsChanged = data.inputParameterChanges->getParameterCount
    (); for (int32 index = 0; index < numParamsChanged; index++)
        {
            if (auto* paramQueue = data.inputParameterChanges->getParameterData
    (index))
            {
                Vst::ParamValue value;
                int32 sampleOffset;
                int32 numPoints = paramQueue->getPointCount ();
                switch (paramQueue->getParameterId ())
                {
                }
            }
        }
    }*/

    //--- Here you have to implement your processing
    if (!(data.numInputs || data.numSamples)) return kResultOk;

    const int32 numInputChannels  = data.inputs[0].numChannels;
    const int32 numOutputChannels = data.outputs[0].numChannels;

    const uint32 sampleFrameSize =
        getSampleFramesSizeInBytes(processSetup, data.numSamples);
    void** in  = getChannelBuffersPointer(processSetup, data.inputs[0]);
    void** out = getChannelBuffersPointer(processSetup, data.outputs[0]);

    if (data.inputs[0].silenceFlags) {
        data.outputs[0].silenceFlags = data.inputs[0].silenceFlags;
        for (int32 i = 0; i < numInputChannels; ++i) {
            if (in[i] != out[i]) {
                memset(out[i], 0, sampleFrameSize);
            }
        }
        return kResultOk;
    }

    data.outputs[0].silenceFlags = 0;

    for (int32 i = 0; i < numOutputChannels; ++i) {
        int32          samples = data.numSamples;
        Vst::Sample32* pIn =
            static_cast<Vst::Sample32*>(in[std::min(i, numInputChannels - 1)]);
        Vst::Sample32* pOut = static_cast<Vst::Sample32*>(out[i]);
        while (--samples >= 0) {
            (*pOut++) = (*pIn++);
        }
    }

    mBuffer.resize(mBuffer.size() + data.numSamples);
    memcpy(mBuffer.data() + mBuffer.size() - data.numSamples,
           in[0],
           data.numSamples * sizeof(float));

    if (mBuffer.size() >= 1024) {
		if (IPtr<Vst::IMessage> msg = owned(allocateMessage()); msg) {
			msg->setMessageID("Data");
			msg->getAttributes()->setBinary(
				"Data", mBuffer.data(), mBuffer.size() * sizeof(float));
			sendMessage(msg);
            mBuffer.clear();
		}
    }

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API
TakoyakiProcessor::setupProcessing(Vst::ProcessSetup& newSetup) {
    //--- called before any processing ----
    return AudioEffect::setupProcessing(newSetup);
}

//------------------------------------------------------------------------
tresult PLUGIN_API
TakoyakiProcessor::canProcessSampleSize(int32 symbolicSampleSize) {
    // by default kSample32 is supported
    if (symbolicSampleSize == Vst::kSample32) return kResultTrue;

    // disable the following comment if your processing support kSample64
    /* if (symbolicSampleSize == Vst::kSample64)
        return kResultTrue; */

    return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API TakoyakiProcessor::setState(IBStream* state) {
    // called when we load a preset, the model has to be reloaded
    IBStreamer streamer(state, kLittleEndian);

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API TakoyakiProcessor::getState(IBStream* state) {
    // here we need to save the model
    IBStreamer streamer(state, kLittleEndian);

    return kResultOk;
}

}  // namespace ht
