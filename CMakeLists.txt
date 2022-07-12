cmake_minimum_required(VERSION 3.23.0)

project(VSTakoyaki
  LANGUAGES C CXX
  VERSION 1.0.0.0
  DESCRIPTION "Takoyaki VST 3 Plug-in"
)

include("cmake/prevent_in_source_builds.cmake")
include("cmake/standard_project_setup.cmake")

include("cmake/compiler_features.cmake")
include("cmake/compiler_warnings.cmake")

include(FetchContent)
include("thirdparty/glfw.cmake")
include("thirdparty/imgui.cmake")
include("thirdparty/imgui-knobs.cmake")
include("thirdparty/vst3sdk.cmake")
add_subdirectory("thirdparty/glad")

add_subdirectory(takoyaki)