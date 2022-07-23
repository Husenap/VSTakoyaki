#pragma once

#include <fstream>

namespace dubu {

struct WriteBuffer {
    virtual ~WriteBuffer()                                   = default;
    virtual void Write(const void* buffer, std::size_t size) = 0;
};

struct ReadBuffer {
    virtual ~ReadBuffer()                             = default;
    virtual void Read(void* buffer, std::size_t size) = 0;
};

struct FileBuffer : WriteBuffer, ReadBuffer {
    enum class Mode { Read, Write };
    FileBuffer(const std::string& file, Mode mode) {
        if (mode == Mode::Read) {
            mStream.open(file, std::ios_base::in | std::ios_base::binary);
        } else {
            mStream.open(file, std::ios_base::out | std::ios_base::binary);
        }
    }
    ~FileBuffer() { mStream.close(); }
    bool         IsOpen() const { return mStream.is_open(); }
    virtual void Write(const void* buffer, std::size_t size) override {
        mStream.write((const char*)buffer, size);
    }
    virtual void Read(void* buffer, std::size_t size) override {
        mStream.read((char*)buffer, size);
    }

private:
    std::fstream mStream;
};

}  // namespace dubu