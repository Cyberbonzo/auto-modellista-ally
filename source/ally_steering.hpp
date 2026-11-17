#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace ally {
struct Steering {
    double value = 0.0;
    static double curve(double input) {
        input = std::clamp(input, -1.0, 1.0);
        const double magnitude = std::abs(input);
        if (magnitude <= 0.08) return 0.0;
        return std::copysign(std::pow((magnitude - 0.08) / 0.92, 1.9), input);
    }
    double update(double input, double seconds) {
        const double target = curve(input);
        const double alpha = -std::expm1(-std::clamp(seconds, 0.0, 0.1) / 0.055);
        value += alpha * (target - value);
        if (target == 0.0 && std::abs(value) < 1.0 / 1024.0) value = 0.0;
        return value;
    }
    std::uint8_t axis(double input, double seconds) {
        const double v = update(input, seconds);
        return static_cast<std::uint8_t>(std::clamp(std::lround(128.0 + v * (v < 0 ? 128.0 : 127.0)), 0L, 255L));
    }
};
}
