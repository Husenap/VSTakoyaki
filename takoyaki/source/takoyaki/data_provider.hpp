#pragma once

#include <atomic>

namespace ht {

template <typename T>
class DataProvider {
public:
    void request() { mFlag = false; }
    void serve() { mFlag = true; }

    T&       data() { return mData; }
    T const& data() const { return mData; }

    T&       operator*() { return mData; }
    T const& operator*() const { return mData; }

    operator bool() const { return mFlag; }

private:
    std::atomic_bool mFlag{false};
    T                mData{};
};

}  // namespace ht