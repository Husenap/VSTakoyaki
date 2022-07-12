#pragma once

#include <complex>
#include <vector>

namespace ht {

static constexpr float PI = 3.1415926535898f;

int reverseBit(int x, int m) {
    int ret = 0;
    for (int k = 0; k < m; ++k) {
        if (x & (1 << k)) ret |= 1 << (m - k - 1);
    }
    return ret;
}

void FFT(std::vector<std::complex<float>> &A) {
    int m = 0;
    while ((1 << m) < static_cast<int>(A.size())) ++m;

    for (int k = 0; k < A.size(); ++k) {
        const int rev = reverseBit(k, m);
        if (k < rev) std::swap(A[k], A[rev]);
    }

    for (int n = 2; n <= A.size(); n <<= 1) {
        for (int k = 0; 2 * k < n; ++k) {
            const auto                a = 2.f * PI * k / n;
            const std::complex<float> x(std::cos(a), std::sin(a));
            for (int j = 0; j < A.size(); j += n) {
                const auto A0k   = A[k + j];
                const auto A1k   = A[k + j + n / 2];
                A[k + j]         = A0k + x * A1k;
                A[k + j + n / 2] = A0k - x * A1k;
            }
        }
    }
}

}  // namespace ht