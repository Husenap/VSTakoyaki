#pragma once

#include <complex>
#include <vector>

namespace ht {

static constexpr float PI = 3.1415926535898f;

std::size_t reverseBit(std::size_t x, std::size_t m) {
    std::size_t ret = 0;
    for (std::size_t k = 0; k < m; ++k) {
        if (x & (1ui64 << k)) ret |= 1ui64 << (m - k - 1);
    }
    return ret;
}

void FFT(std::vector<std::complex<float>> &A) {
    std::size_t m = 0;
    while ((1 << m) < static_cast<int>(A.size())) ++m;

    for (std::size_t k = 0; k < A.size(); ++k) {
        const std::size_t rev = reverseBit(k, m);
        if (k < rev) std::swap(A[k], A[rev]);
    }

    for (std::size_t n = 2; n <= A.size(); n <<= 1) {
        for (std::size_t k = 0; 2 * k < n; ++k) {
            const auto                a = 2.f * PI * k / n;
            const std::complex<float> x(std::cos(a), std::sin(a));
            for (std::size_t j = 0; j < A.size(); j += n) {
                const auto A0k   = A[k + j];
                const auto A1k   = A[k + j + n / 2];
                A[k + j]         = A0k + x * A1k;
                A[k + j + n / 2] = A0k - x * A1k;
            }
        }
    }
}

}  // namespace ht