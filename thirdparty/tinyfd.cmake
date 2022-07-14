message(STATUS "Fetching External Project: tinyfd")

FetchContent_Declare(
  tinyfd
  GIT_REPOSITORY https://git.code.sf.net/p/tinyfiledialogs/code
  GIT_TAG e11f94cd7887b101d64f74892d769f0139b5e166
  SOURCE_DIR "${FETCHCONTENT_BASE_DIR}/tinyfd/tinyfd"
)

FetchContent_MakeAvailable(tinyfd)

set(tinyfd_sources
  ${tinyfd_SOURCE_DIR}/tinyfiledialogs.c
  ${tinyfd_SOURCE_DIR}/tinyfiledialogs.h)

add_library(tinyfd STATIC ${tinyfd_sources})

target_include_directories(tinyfd SYSTEM INTERFACE
  "${tinyfd_SOURCE_DIR}"
  "${tinyfd_SOURCE_DIR}/..")
set_target_properties(tinyfd PROPERTIES FOLDER "thirdparty/tinyfd")