message("-- External Project: glad")

add_library(glad STATIC "include/glad/gl.h" "src/gl.c")
target_include_directories(glad PUBLIC "include/")
set_target_properties(glad PROPERTIES FOLDER "thirdparty/glad")