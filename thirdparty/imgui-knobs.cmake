message("-- Fetching External Project: imgui-knobs")

FetchContent_Declare(
  imgui-knobs
  GIT_REPOSITORY https://github.com/altschuler/imgui-knobs.git
  GIT_TAG 1d31738d31f7f1840328de4a00756f58f668eb90
  SOURCE_DIR "${FETCHCONTENT_BASE_DIR}/imgui-knobs/imgui-knobs"
)

FetchContent_GetProperties(imgui-knobs)

if(NOT imgui-knobs_POPULATED)
  FetchContent_Populate(imgui-knobs)

  set(src_imgui_knobs
    "${imgui-knobs_SOURCE_DIR}/imgui-knobs.cpp"
    "${imgui-knobs_SOURCE_DIR}/imgui-knobs.h"
  )

  add_library(imgui-knobs STATIC ${src_imgui_knobs})

  target_include_directories(imgui-knobs SYSTEM PUBLIC
    "${imgui-knobs_SOURCE_DIR}"
    "${imgui-knobs_SOURCE_DIR}/..")

  target_link_libraries(imgui-knobs imgui)

  set_target_properties(imgui-knobs PROPERTIES FOLDER "thirdparty/imgui-knobs")
endif()
