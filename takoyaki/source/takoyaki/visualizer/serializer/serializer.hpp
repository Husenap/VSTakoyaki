#pragma once

#include <vector>

#include "buffer.hpp"

namespace dubu {

template <typename T, bool IsPod>
struct Serializer {
    void Read(T& object, ReadBuffer& buffer) { object.Serialize(buffer); }
    void Write(const T& object, WriteBuffer& buffer) {
        object.Serialize(buffer);
    }
};

template <typename T>
WriteBuffer& operator<<(WriteBuffer& buffer, const T& object) {
    Serializer<T, std::is_trivial_v<std::decay_t<T>>>().Write(object, buffer);
    return buffer;
}

template <typename T>
ReadBuffer& operator>>(ReadBuffer& buffer, T& object) {
    Serializer<T, std::is_trivial_v<std::decay_t<T>>>().Read(object, buffer);
    return buffer;
}

template <typename T>
struct Serializer<T, true> {
    void Read(T& object, ReadBuffer& buffer) {
        buffer.Read(&object, sizeof(object));
    }
    void Write(const T& object, WriteBuffer& buffer) {
        buffer.Write(&object, sizeof(object));
    }
};

template <>
struct Serializer<std::string, false> {
    void Read(std::string& object, ReadBuffer& buffer) {
        std::size_t size;
        buffer >> size;
        object.resize(size);
        buffer.Read(object.data(), size);
    }
    void Write(const std::string& object, WriteBuffer& buffer) {
        std::size_t size = object.size();
        buffer << size;
        buffer.Write(object.data(), size);
    }
};

template <typename T>
struct Serializer<std::vector<T>, false> {
    void Read(std::vector<T>& object, ReadBuffer& buffer) {
        std::size_t size;
        buffer >> size;
        object.resize(size);
        for (std::size_t i = 0; i < size; ++i) {
            buffer >> object[i];
        }
    }
    void Write(const std::vector<T>& object, WriteBuffer& buffer) {
        std::size_t size = object.size();
        buffer << size;
        for (std::size_t i = 0; i < size; ++i) {
            buffer << object[i];
        }
    }
};

}  // namespace dubu