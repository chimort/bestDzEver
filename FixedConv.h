#pragma once

#include "Fixed.h"
#include "FastFixed.h"

// Conversion from FAST_FIXED to Fixed
template <size_t N, size_t K>
Fixed<N, K> toFixed(const FAST_FIXED<N, K>& ff) {
    return Fixed<N, K>(static_cast<double>(ff));
}

// Conversion from Fixed to FAST_FIXED
template <size_t N, size_t K>
FAST_FIXED<N, K> toFAST_FIXED(const Fixed<N, K>& f) {
    return FAST_FIXED<N, K>(static_cast<double>(f));
}

// Arithmetic Operators between Fixed and FAST_FIXED

// Addition
template <size_t N, size_t K>
Fixed<N, K> operator+(const Fixed<N, K>& a, const FAST_FIXED<N, K>& b) {
    return a + toFixed(b);
}

template <size_t N, size_t K>
Fixed<N, K> operator+(const FAST_FIXED<N, K>& a, const Fixed<N, K>& b) {
    return toFixed(a) + b;
}

// Subtraction
template <size_t N, size_t K>
Fixed<N, K> operator-(const Fixed<N, K>& a, const FAST_FIXED<N, K>& b) {
    return a - toFixed(b);
}

template <size_t N, size_t K>
Fixed<N, K> operator-(const FAST_FIXED<N, K>& a, const Fixed<N, K>& b) {
    return toFixed(a) - b;
}

// Multiplication
template <size_t N, size_t K>
Fixed<N, K> operator*(const Fixed<N, K>& a, const FAST_FIXED<N, K>& b) {
    return a * toFixed(b);
}

template <size_t N, size_t K>
Fixed<N, K> operator*(const FAST_FIXED<N, K>& a, const Fixed<N, K>& b) {
    return toFixed(a) * b;
}

// Division
template <size_t N, size_t K>
Fixed<N, K> operator/(const Fixed<N, K>& a, const FAST_FIXED<N, K>& b) {
    return a / toFixed(b);
}

template <size_t N, size_t K>
Fixed<N, K> operator/(const FAST_FIXED<N, K>& a, const Fixed<N, K>& b) {
    return toFixed(a) / b;
}

// Comparison Operators between FAST_FIXED and Fixed

// Less Than or Equal To
template <size_t N, size_t K>
bool operator<=(const FAST_FIXED<N, K>& a, const Fixed<N, K>& b) {
    return static_cast<double>(a) <= static_cast<double>(b);
}

template <size_t N, size_t K>
bool operator<=(const Fixed<N, K>& a, const FAST_FIXED<N, K>& b) {
    return static_cast<double>(a) <= static_cast<double>(b);
}

// Greater Than or Equal To
template <size_t N, size_t K>
bool operator>=(const FAST_FIXED<N, K>& a, const Fixed<N, K>& b) {
    return static_cast<double>(a) >= static_cast<double>(b);
}

template <size_t N, size_t K>
bool operator>=(const Fixed<N, K>& a, const FAST_FIXED<N, K>& b) {
    return static_cast<double>(a) >= static_cast<double>(b);
}

// Equality
template <size_t N, size_t K>
bool operator==(const FAST_FIXED<N, K>& a, const Fixed<N, K>& b) {
    return static_cast<double>(a) == static_cast<double>(b);
}

template <size_t N, size_t K>
bool operator==(const Fixed<N, K>& a, const FAST_FIXED<N, K>& b) {
    return static_cast<double>(a) == static_cast<double>(b);
}

// Inequality
template <size_t N, size_t K>
bool operator!=(const FAST_FIXED<N, K>& a, const Fixed<N, K>& b) {
    return static_cast<double>(a) != static_cast<double>(b);
}

template <size_t N, size_t K>
bool operator!=(const Fixed<N, K>& a, const FAST_FIXED<N, K>& b) {
    return static_cast<double>(a) != static_cast<double>(b);
}

// Assignment Operators

// Operator -= for Fixed and FAST_FIXED
template <size_t N, size_t K>
Fixed<N, K>& operator-=(Fixed<N, K>& a, const FAST_FIXED<N, K>& b) {
    a = a - b;
    return a;
}

template <size_t N, size_t K>
Fixed<N, K>& operator*=(Fixed<N, K>& a, const FAST_FIXED<N, K>& b) {
    a = a * b;
    return a;
}

template <size_t N, size_t K>
FAST_FIXED<N, K>& operator-=(FAST_FIXED<N, K>& a, const Fixed<N, K>& b) {
    a = toFAST_FIXED(toFixed(a) - b);
    return a;
}

template <size_t N, size_t K>
FAST_FIXED<N, K>& operator*=(FAST_FIXED<N, K>& a, const Fixed<N, K>& b) {
    a = toFAST_FIXED(toFixed(a) * b);
    return a;
}