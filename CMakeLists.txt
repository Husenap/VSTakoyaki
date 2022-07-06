cmake_minimum_required(VERSION 3.23.0)

project(VSTakoyaki
  LANGUAGES C CXX
  VERSION 1.0.0.0
  DESCRIPTION "Takoyaki VST 3 Plug-in"
)

include("thirdparty/vst3sdk.cmake")
add_subdirectory(takoyaki)