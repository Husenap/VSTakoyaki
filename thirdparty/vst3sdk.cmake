message("-- External Project: vst3sdk")

FetchContent_Declare(
  vst3sdk
  GIT_REPOSITORY https://github.com/steinbergmedia/vst3sdk.git
  GIT_TAG v3.7.5_build_44
)

FetchContent_MakeAvailable(vst3sdk)
smtg_enable_vst3_sdk()