#pragma once
#include "Fixed.h"

struct ConfigFixed16 {
    using PressureType = Fixed<16, 16>;
    using VelocityType = Fixed<16, 16>;
    using VelocityFlowType = Fixed<16, 16>;
};

struct ConfigFixed32 {
    using PressureType = Fixed<32, 16>;
    using VelocityType = Fixed<32, 16>;
    using VelocityFlowType = Fixed<32, 16>;
};

struct ConfigFixed64 {
    using PressureType = Fixed<64, 32>;
    using VelocityType = Fixed<64, 32>;
    using VelocityFlowType = Fixed<64, 32>;
};

struct ConfigFloat {
    using PressureType = float;
    using VelocityType = float;
    using VelocityFlowType = float;
};

struct ConfigDouble {
    using PressureType = double;
    using VelocityType = double;
    using VelocityFlowType = double;
};

template <size_t N, size_t K>
struct ConfigFastFixed
{
    using PressureType = FAST_FIXED<N, K>;
    using VelocityType = FAST_FIXED<N, K>;
    using VelocityFlowType = FAST_FIXED<N, K>;
};

