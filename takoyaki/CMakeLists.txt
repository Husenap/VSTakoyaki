set(target_name "takoyaki")

smtg_add_vst3plugin(${target_name}
    source/version.h
    source/takoyakicids.h
    source/takoyakiprocessor.h
    source/takoyakiprocessor.cpp
    source/takoyakicontroller.h
    source/takoyakicontroller.cpp
    source/takoyakientry.cpp
)

target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

smtg_target_add_plugin_snapshots(${target_name}
    RESOURCES
    resource/0EDA05DC31765755983A30917D83FD5F_snapshot.png
    resource/0EDA05DC31765755983A30917D83FD5F_snapshot_2.0x.png
)

target_link_libraries(${target_name}
    PRIVATE sdk
)

smtg_target_configure_version_file(${target_name})

target_sources(${target_name} PRIVATE resource/win32resource.rc)

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${target_name})

    smtg_target_set_debug_executable(${target_name}
        "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
        "--pluginfolder \"$(OutDir)/\""
    )
endif()
