message(STATUS "Fetching External Project: glfw")

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.7
)

option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_DOCS OFF)
option(GLFW_INSTALL OFF)

FetchContent_MakeAvailable(glfw)

set_target_properties(glfw PROPERTIES FOLDER "thirdparty/glfw")