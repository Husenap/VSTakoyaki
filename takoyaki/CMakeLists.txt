set(target_name "takoyaki")

smtg_add_vst3plugin(${target_name}
    "source/takoyaki/data_provider.hpp"
    "source/takoyaki/fft.hpp"
    "source/takoyaki/imgui_wrapper.hpp"
    "source/takoyaki/imgui_wrapper.cpp"
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

target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source)

target_sources(${target_name}
    PRIVATE
    resource/imgui.ini
)

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
    sdk
    imgui
    imgui-knobs
    glfw
)

smtg_target_configure_version_file(${target_name})

target_sources(${target_name} PRIVATE resource/win32resource.rc)

target_compile_definitions(${target_name} PRIVATE -DNOMINMAX)

smtg_target_set_debug_executable(${target_name}
    "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
    "--pluginfolder \"$(OutDir)/\""
)
