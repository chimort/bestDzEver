#pragma once

#include <cstdint>
#include <iostream>
#include <compare>
#include <type_traits>

struct RawTag {};

template <size_t N, size_t K>
struct Fixed {
    static_assert(N <= 64, "FIXED<N, K> поддерживает максимум 64 бита.");
    static_assert(N > 0, "FIXED<N, K> должно содержать хотя бы 1 бит.");
    static_assert(K < N, "Количество битов дробной части должно быть меньше общего количества битов.");

    using StorageType = std::conditional_t<
        (N <= 8), int8_t,
        std::conditional_t<
            (N <= 16), int16_t,
            std::conditional_t<
                (N <= 32), int32_t,
                int64_t
            >
        >
    >;

    StorageType v;

    Fixed() : v(0) {}
    constexpr Fixed(int32_t value) : v(static_cast<StorageType>(value) << K) {}
    constexpr Fixed(float f) : v(static_cast<StorageType>(f * (1ULL << K))) {}
    constexpr Fixed(double f) : v(static_cast<StorageType>(f * (1ULL << K))) {}

    constexpr Fixed(RawTag, StorageType raw) : v(raw) {}


public:
    static constexpr Fixed from_raw(StorageType raw) {
        return Fixed(RawTag{}, raw);
    }

    explicit operator float() const {
        return static_cast<float>(v) / (1ULL << K);
    }

    explicit operator double() const {
        return static_cast<double>(v) / (1ULL << K);
    }

    auto operator<=>(const Fixed&) const = default;
    bool operator==(const Fixed&) const = default;
};

template <size_t N, size_t K>
Fixed<N, K> operator+(const Fixed<N, K>& a, const Fixed<N, K>& b) {
    return Fixed<N, K>::from_raw(a.v + b.v);
}

template <size_t N, size_t K>
Fixed<N, K> operator-(const Fixed<N, K>& a, const Fixed<N, K>& b) {
    return Fixed<N, K>::from_raw(a.v - b.v);
}

template <size_t N, size_t K>
Fixed<N, K> operator*(const Fixed<N, K>& a, const Fixed<N, K>& b) {
    using IntermediateType = std::conditional_t<
        (N <= 16), int32_t,
        int64_t
    >;
    IntermediateType temp = static_cast<IntermediateType>(a.v) * static_cast<IntermediateType>(b.v);
    return Fixed<N, K>::from_raw(static_cast<typename Fixed<N, K>::StorageType>(temp >> K));
}

template <size_t N, size_t K>
Fixed<N, K> operator/(const Fixed<N, K>& a, const Fixed<N, K>& b) {
    using IntermediateType = std::conditional_t<
        (N <= 16), int32_t,
        int64_t
    >;
    IntermediateType temp = (static_cast<IntermediateType>(a.v) << K) / b.v;
    return Fixed<N, K>::from_raw(static_cast<typename Fixed<N, K>::StorageType>(temp));
}

template <size_t N, size_t K>
Fixed<N, K>& operator+=(Fixed<N, K>& a, const Fixed<N, K>& b) {
    a = a + b;
    return a;
}

template <size_t N, size_t K>
Fixed<N, K>& operator-=(Fixed<N, K>& a, const Fixed<N, K>& b) {
    a = a - b;
    return a;
}

template <size_t N, size_t K>
Fixed<N, K>& operator*=(Fixed<N, K>& a, const Fixed<N, K>& b) {
    a = a * b;
    return a;
}

template <size_t N, size_t K>
Fixed<N, K>& operator/=(Fixed<N, K>& a, const Fixed<N, K>& b) {
    a = a / b;
    return a;
}

template <size_t N, size_t K>
std::ostream& operator<<(std::ostream& os, const Fixed<N, K>& fixed) {
    os << static_cast<double>(fixed);
    return os;
}

template <size_t N, size_t K>
bool operator>(const Fixed<N, K>& a, int b) {
    return static_cast<double>(a) > static_cast<double>(b);
}

template <size_t N, size_t K>
bool operator<(const Fixed<N, K>& a, int b) {
    return static_cast<double>(a) < static_cast<double>(b);
}

template <size_t N, size_t K>
bool operator>=(const Fixed<N, K>& a, int b) {
    return static_cast<double>(a) >= static_cast<double>(b);
}

template <size_t N, size_t K>
bool operator<=(const Fixed<N, K>& a, int b) {
    return static_cast<double>(a) <= static_cast<double>(b);
}

template <size_t N, size_t K>
bool operator==(const Fixed<N, K>& a, int b) {
    return static_cast<double>(a) == static_cast<double>(b);
}

template <size_t N, size_t K>
bool operator!=(const Fixed<N, K>& a, int b) {
    return static_cast<double>(a) != static_cast<double>(b);
}

template <size_t N, size_t K>
bool operator>(const Fixed<N, K>& a, double b) {
    return static_cast<double>(a) > b;
}

template <size_t N, size_t K>
bool operator<(const Fixed<N, K>& a, double b) {
    return static_cast<double>(a) < b;
}

template <size_t N, size_t K>
bool operator>=(const Fixed<N, K>& a, double b) {
    return static_cast<double>(a) >= b;
}

template <size_t N, size_t K>
bool operator<=(const Fixed<N, K>& a, double b) {
    return static_cast<double>(a) <= b;
}

template <size_t N, size_t K>
bool operator==(const Fixed<N, K>& a, double b) {
    return static_cast<double>(a) == b;
}

template <size_t N, size_t K>
bool operator!=(const Fixed<N, K>& a, double b) {
    return static_cast<double>(a) != b;
}

template <size_t N, size_t K>
Fixed<N, K> operator*(const Fixed<N, K>& a, double b) {
    return Fixed<N, K>(static_cast<double>(a) * b);
}

template <size_t N, size_t K>
Fixed<N, K> operator/(const Fixed<N, K>& a, double b) {
    return Fixed<N, K>(static_cast<double>(a) / b);
}

template <size_t N, size_t K>
Fixed<N, K>& operator*=(Fixed<N, K>& a, double b) {
    a = static_cast<double>(a) * b;
    return a;
}

template <size_t N, size_t K>
Fixed<N, K>& operator/=(Fixed<N, K>& a, double b) {
    a = static_cast<double>(a) / b;
    return a;
}