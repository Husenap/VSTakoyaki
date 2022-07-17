#pragma once

// win32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// STL
#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <variant>
#include <vector>

// variant helper
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

// GL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// tinyfd
#include <tinyfiledialogs.h>

// takoyaki includes
#include "takoyaki/visualizer/editor/handlers.hpp"
#include "takoyaki/visualizer/graphics/gl/render_command.hpp"
#include "takoyaki/visualizer/graphics/gl/render_target.hpp"
#include "takoyaki/visualizer/graphics/gl/shader.hpp"
#include "takoyaki/visualizer/graphics/gl/shader_program.hpp"
#include "takoyaki/visualizer/serializer/serializer.hpp"
#include "takoyaki/visualizer/util/file_watcher.hpp"
#include "takoyaki/visualizer/util/input.hpp"
