#pragma once

#include <complex>
#include <vector>

namespace ht {

// Todo: Implement the inplace FFT algortihm
void FFT(std::vector<std::complex<float>> &A) {
    const auto n = A.size();
    if (n <= 1) return;

    std::vector<std::complex<float>> A0(n / 2), A1(n / 2);
    for (std::size_t k = 0; k * 2 < n; ++k) {
        A0[k] = A[2 * k];
        A1[k] = A[2 * k + 1];
    }

    FFT(A0);
    FFT(A1);

    for (std::size_t k = 0; k * 2 < n; ++k) {
        static constexpr float PI = 3.1415926535898f;
        std::complex<float>    x(std::cos(2 * PI * k / n),
                              std::sin(2 * PI * k / n));
        A[k]         = A0[k] + x * A1[k];
        A[k + n / 2] = A0[k] - x * A1[k];
    }
}

}  // namespace ht