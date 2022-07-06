//------------------------------------------------------------------------
// Copyright(c) 2022 Hussein Taher.
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace ht {
//------------------------------------------------------------------------
static const Steinberg::FUID kTakoyakiProcessorUID(0x0EDA05DC,
                                                   0x31765755,
                                                   0x983A3091,
                                                   0x7D83FD5F);
static const Steinberg::FUID kTakoyakiControllerUID(0x8DEE7DDD,
                                                    0x35455BC7,
                                                    0xBFD4DF1D,
                                                    0xCC1DB743);

#define TakoyakiVST3Category "Fx"

//------------------------------------------------------------------------
}  // namespace ht
