set(target_name "takoyaki")

set(src_files
    "source/takoyaki/visualizer/editor/widgets/camera.cpp"
    "source/takoyaki/visualizer/editor/widgets/camera.hpp"
    "source/takoyaki/visualizer/editor/widgets/widget.cpp"
    "source/takoyaki/visualizer/editor/widgets/widget.hpp"
    "source/takoyaki/visualizer/graphics/gl/render_command.hpp"
    "source/takoyaki/visualizer/graphics/gl/render_commands.hpp"
    "source/takoyaki/visualizer/graphics/gl/render_target.cpp"
    "source/takoyaki/visualizer/graphics/gl/render_target.hpp"
    "source/takoyaki/visualizer/graphics/gl/shader_program.hpp"
    "source/takoyaki/visualizer/graphics/gl/shader.hpp"
    "source/takoyaki/visualizer/graphics/base_window.hpp"
    "source/takoyaki/visualizer/graphics/main_window.cpp"
    "source/takoyaki/visualizer/graphics/main_window.hpp"
    "source/takoyaki/visualizer/graphics/renderer.cpp"
    "source/takoyaki/visualizer/graphics/renderer.hpp"
    "source/takoyaki/visualizer/util/file_watcher.cpp"
    "source/takoyaki/visualizer/util/file_watcher.hpp"
    "source/takoyaki/visualizer/util/input.hpp"
    "source/takoyaki/visualizer/visualizer.cpp"
    "source/takoyaki/visualizer/visualizer.hpp"
    "source/takoyaki/data_provider.hpp"
    "source/takoyaki/fft.hpp"
    "source/takoyaki/imgui_wrapper.cpp"
    "source/takoyaki/imgui_wrapper.hpp"
    "source/takoyaki/window.cpp"
    "source/takoyaki/window.hpp"
    "source/takoyakicids.h"
    "source/takoyakicontroller.cpp"
    "source/takoyakicontroller.h"
    "source/takoyakientry.cpp"
    "source/takoyakiprocessor.cpp"
    "source/takoyakiprocessor.h"
    "source/version.h"
)

smtg_add_vst3plugin(${target_name} ${src_files})

auto_group_sources(src_files ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source)

smtg_target_add_plugin_resources(${target_name}
    RESOURCES
    "resource/imgui.ini"
    "resource/Roboto-Regular.ttf"
)

smtg_target_add_plugin_snapshots(${target_name}
    RESOURCES
    resource/0EDA05DC31765755983A30917D83FD5F_snapshot.png
    resource/0EDA05DC31765755983A30917D83FD5F_snapshot_2.0x.png
)

target_link_libraries(${target_name}
    PRIVATE
    compiler_features
    compiler_warnings
    glm::glm
    glad
    glfw
    imgui
    imgui-knobs
    sdk
)

smtg_target_configure_version_file(${target_name})

target_sources(${target_name} PRIVATE resource/win32resource.rc)

target_compile_definitions(${target_name}
    PRIVATE
    -DNOMINMAX
    -DGLFW_INCLUDE_NONE
)

smtg_target_set_debug_executable(${target_name}
    "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
    "--pluginfolder \"$(OutDir)/\""
)
