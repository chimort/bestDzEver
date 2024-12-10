#pragma once

#include <cstdint>
#include <iostream>
#include <type_traits>
#include <compare>

template <size_t N, size_t K>
struct FAST_FIXED {
    using FastInt = typename std::conditional_t<
        (N <= 8), int_fast8_t,
        std::conditional_t<
            (N <= 16), int_fast16_t,
            std::conditional_t<
                (N <= 32), int_fast32_t,
                int_fast64_t
            >
        >
    >;

    static_assert(sizeof(FastInt) * 8 >= N, "FAST_FIXED<N, K>: недостаточное количество бит для FastInt.");

    FastInt v;

    FAST_FIXED() : v(0) {}
    constexpr FAST_FIXED(int32_t value) : v(static_cast<FastInt>(value) << K) {}
    constexpr FAST_FIXED(float f) : v(static_cast<FastInt>(f * (1ULL << K))) {}
    constexpr FAST_FIXED(double f) : v(static_cast<FastInt>(f * (1ULL << K))) {}
    constexpr FAST_FIXED(FastInt raw) : v(raw) {}

    operator float() const {
        return static_cast<float>(v) / (1ULL << K);
    }

    operator double() const {
        return static_cast<double>(v) / (1ULL << K);
    }

    auto operator<=>(const FAST_FIXED&) const = default;
    bool operator==(const FAST_FIXED&) const = default;
};

template <size_t N, size_t K>
FAST_FIXED<N, K> operator+(const FAST_FIXED<N, K>& a, const FAST_FIXED<N, K>& b) {
    return FAST_FIXED<N, K>(a.v + b.v);
}

template <size_t N, size_t K>
FAST_FIXED<N, K> operator-(const FAST_FIXED<N, K>& a, const FAST_FIXED<N, K>& b) {
    return FAST_FIXED<N, K>(a.v - b.v);
}

template <size_t N, size_t K>
FAST_FIXED<N, K> operator*(const FAST_FIXED<N, K>& a, const FAST_FIXED<N, K>& b) {
    return FAST_FIXED<N, K>((static_cast<int64_t>(a.v) * b.v) >> K);
}

template <size_t N, size_t K>
FAST_FIXED<N, K> operator/(const FAST_FIXED<N, K>& a, const FAST_FIXED<N, K>& b) {
    return FAST_FIXED<N, K>((static_cast<int64_t>(a.v) << K) / b.v);
}

template <size_t N, size_t K>
std::ostream& operator<<(std::ostream& os, const FAST_FIXED<N, K>& fixed) {
    os << static_cast<double>(fixed);
    return os;
}