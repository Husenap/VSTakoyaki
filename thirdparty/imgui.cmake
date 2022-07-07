message("-- Fetching External Project: imgui")

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG 7b5a8e4f2bbc06eac3b913a7f7df67edff99c538
  SOURCE_DIR "${FETCHCONTENT_BASE_DIR}/imgui/imgui"
)

FetchContent_GetProperties(imgui)

if(NOT imgui_POPULATED)
  FetchContent_Populate(imgui)

  set(src_imgui
    "${imgui_SOURCE_DIR}/imconfig.h"
    "${imgui_SOURCE_DIR}/imgui_demo.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_internal.h"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui.h"
    "${imgui_SOURCE_DIR}/imstb_rectpack.h"
    "${imgui_SOURCE_DIR}/imstb_textedit.h"
    "${imgui_SOURCE_DIR}/imstb_truetype.h"
    "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
    "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h"

    # backend implementation: GLFW + OpenGL3
    "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3_loader.h"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h"
  )

  add_library(imgui STATIC ${src_imgui})

  target_include_directories(imgui SYSTEM PUBLIC
    "${imgui_SOURCE_DIR}"
    "${imgui_SOURCE_DIR}/..")

  target_compile_definitions(imgui PRIVATE -DGLFW_INCLUDE_NONE)
  target_link_libraries(imgui glfw)

  set_target_properties(imgui PROPERTIES FOLDER "thirdparty/imgui")
endif()
