#pragma once

#include <cstdint>
#include <iostream>
#include <type_traits>
#include <compare>

struct Raw {};

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

    static_assert(sizeof(FastInt) * 8 >= N, "FAST_FIXED: недостаточное количество бит для FastInt.");

    FastInt v;

    FAST_FIXED() : v(0) {}
    constexpr FAST_FIXED(int32_t value) : v(static_cast<FastInt>(value) << K) {}
    constexpr FAST_FIXED(float f) : v(static_cast<FastInt>(f * (1ULL << K))) {}
    constexpr FAST_FIXED(double f) : v(static_cast<FastInt>(f * (1ULL << K))) {}

    constexpr FAST_FIXED(Raw, FastInt raw) : v(raw) {}

public:
    static constexpr FAST_FIXED from_raw(FastInt raw) {
        return FAST_FIXED(Raw{}, raw);
    }

    explicit operator float() const {
        return static_cast<float>(v) / (1ULL << K);
    }

    explicit operator double() const {
        return static_cast<double>(v) / (1ULL << K);
    }

    auto operator<=>(const FAST_FIXED&) const = default;
    bool operator==(const FAST_FIXED&) const = default;
};

template <size_t N, size_t K>
FAST_FIXED<N, K> operator+(const FAST_FIXED<N, K>& a, const FAST_FIXED<N, K>& b) {
    return FAST_FIXED<N, K>::from_raw(a.v + b.v);
}

template <size_t N, size_t K>
FAST_FIXED<N, K> operator-(const FAST_FIXED<N, K>& a, const FAST_FIXED<N, K>& b) {
    return FAST_FIXED<N, K>::from_raw(a.v - b.v);
}

template <size_t N, size_t K>
FAST_FIXED<N, K> operator*(const FAST_FIXED<N, K>& a, const FAST_FIXED<N, K>& b) {
    using IntermediateType = std::conditional_t<
        (N <= 16), int_fast32_t,
        int_fast64_t
    >;
    IntermediateType temp = static_cast<IntermediateType>(a.v) * static_cast<IntermediateType>(b.v);
    return FAST_FIXED<N, K>::from_raw(static_cast<typename FAST_FIXED<N, K>::FastInt>(temp >> K));
}

template <size_t N, size_t K>
FAST_FIXED<N, K> operator/(const FAST_FIXED<N, K>& a, const FAST_FIXED<N, K>& b) {
    using IntermediateType = std::conditional_t<
        (N <= 16), int_fast32_t,
        int_fast64_t
    >;
    IntermediateType temp = (static_cast<IntermediateType>(a.v) << K) / b.v;
    return FAST_FIXED<N, K>::from_raw(static_cast<typename FAST_FIXED<N, K>::FastInt>(temp));
}

template <size_t N, size_t K>
FAST_FIXED<N, K>& operator+=(FAST_FIXED<N, K>& a, const FAST_FIXED<N, K>& b) {
    a = a + b;
    return a;
}

template <size_t N, size_t K>
FAST_FIXED<N, K>& operator-=(FAST_FIXED<N, K>& a, const FAST_FIXED<N, K>& b) {
    a = a - b;
    return a;
}

template <size_t N, size_t K>
FAST_FIXED<N, K>& operator*=(FAST_FIXED<N, K>& a, const FAST_FIXED<N, K>& b) {
    a = a * b;
    return a;
}

template <size_t N, size_t K>
FAST_FIXED<N, K>& operator/=(FAST_FIXED<N, K>& a, const FAST_FIXED<N, K>& b) {
    a = a / b;
    return a;
}

template <size_t N, size_t K>
std::ostream& operator<<(std::ostream& os, const FAST_FIXED<N, K>& fixed) {
    os << static_cast<double>(fixed);
    return os;
}

template <size_t N, size_t K>
bool operator>(const FAST_FIXED<N, K>& a, int b) {
    return static_cast<double>(a) > static_cast<double>(b);
}

template <size_t N, size_t K>
bool operator<(const FAST_FIXED<N, K>& a, int b) {
    return static_cast<double>(a) < static_cast<double>(b);
}

template <size_t N, size_t K>
bool operator>=(const FAST_FIXED<N, K>& a, int b) {
    return static_cast<double>(a) >= static_cast<double>(b);
}

template <size_t N, size_t K>
bool operator<=(const FAST_FIXED<N, K>& a, int b) {
    return static_cast<double>(a) <= static_cast<double>(b);
}

template <size_t N, size_t K>
bool operator==(const FAST_FIXED<N, K>& a, int b) {
    return static_cast<double>(a) == static_cast<double>(b);
}

template <size_t N, size_t K>
bool operator!=(const FAST_FIXED<N, K>& a, int b) {
    return static_cast<double>(a) != static_cast<double>(b);
}

template <size_t N, size_t K>
bool operator>(const FAST_FIXED<N, K>& a, double b) {
    return static_cast<double>(a) > b;
}

template <size_t N, size_t K>
bool operator<(const FAST_FIXED<N, K>& a, double b) {
    return static_cast<double>(a) < b;
}

template <size_t N, size_t K>
bool operator>=(const FAST_FIXED<N, K>& a, double b) {
    return static_cast<double>(a) >= b;
}

template <size_t N, size_t K>
bool operator<=(const FAST_FIXED<N, K>& a, double b) {
    return static_cast<double>(a) <= b;
}

template <size_t N, size_t K>
bool operator==(const FAST_FIXED<N, K>& a, double b) {
    return static_cast<double>(a) == b;
}

template <size_t N, size_t K>
bool operator!=(const FAST_FIXED<N, K>& a, double b) {
    return static_cast<double>(a) != b;
}

template <size_t N, size_t K>
FAST_FIXED<N, K> operator*(const FAST_FIXED<N, K>& a, double b) {
    return FAST_FIXED<N, K>(static_cast<double>(a) * b);
}

template <size_t N, size_t K>
FAST_FIXED<N, K> operator/(const FAST_FIXED<N, K>& a, double b) {
    return FAST_FIXED<N, K>(static_cast<double>(a) / b);
}

template <size_t N, size_t K>
FAST_FIXED<N, K>& operator*=(FAST_FIXED<N, K>& a, double b) {
    a = static_cast<double>(a) * b;
    return a;
}

template <size_t N, size_t K>
FAST_FIXED<N, K>& operator/=(FAST_FIXED<N, K>& a, double b) {
    a = static_cast<double>(a) / b;
    return a;
}