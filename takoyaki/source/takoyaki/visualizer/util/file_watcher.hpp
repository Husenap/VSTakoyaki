#pragma once

#include <filesystem>
#include <functional>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

namespace ht {

using FileChangeCallback = std::function<void(const std::string&)>;

class FileWatcher {
public:
    FileWatcher();
    ~FileWatcher();

    void Flush();

    void Watch(const std::string& file, FileChangeCallback callback);
    void Clear();

private:
    void Job();

    struct FileData {
        std::string                     mFilePath;
        FileChangeCallback              mCallback;
        std::filesystem::file_time_type mLastWriteTime;
    };

    std::unordered_map<std::string, FileData> mFiles;
    std::vector<std::string>                  mMarkedFiles;

    std::jthread mThread;
    std::mutex   mMutex;
};

}  // namespace ht