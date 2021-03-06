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

include("cmake/auto_group_sources.cmake")

include(FetchContent)
add_subdirectory("thirdparty/glad")
include("thirdparty/glm.cmake")
include("thirdparty/glfw.cmake")
include("thirdparty/imgui.cmake")
include("thirdparty/imgui-knobs.cmake")
include("thirdparty/tinyfd.cmake")
include("thirdparty/vst3sdk.cmake")

add_subdirectory(takoyaki)