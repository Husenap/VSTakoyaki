#pragma once

#include <filesystem>
#include <functional>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

namespace ht {

using FileChangeCallback = std::function<void(const std::filesystem::path&)>;

class FileWatcher {
public:
    FileWatcher();
    ~FileWatcher();

    void Flush();

    void Watch(const std::filesystem::path& file, FileChangeCallback callback);
    void Clear();

private:
    void Job();

    struct FileData {
        std::filesystem::path           mFilePath;
        FileChangeCallback              mCallback;
        std::filesystem::file_time_type mLastWriteTime;
    };

    std::unordered_map<std::filesystem::path, FileData> mFiles;
    std::vector<std::filesystem::path>                  mMarkedFiles;

    std::jthread mThread;
    std::mutex   mMutex;
};

}  // namespace ht