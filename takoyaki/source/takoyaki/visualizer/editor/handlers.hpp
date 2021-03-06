#pragma once

namespace ht {

// File handlers
using NewFileHandler  = std::function<void()>;
using OpenFileHandler = std::function<void()>;
using SaveFileHandler = std::function<void()>;

// Editor handlers
using UniformsChangedHandler = std::function<void()>;

}  // namespace ht