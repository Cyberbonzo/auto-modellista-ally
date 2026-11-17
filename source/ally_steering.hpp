#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace ally {
struct Steering {
    double value = 0.0;
    static double curve(double input, double kmh = 0) {
        const double magnitude = std::clamp(std::abs(input), 0.0, 1.0);
        if (magnitude <= 0.10) return 0.0;
        const double speed = std::clamp((kmh - 45.0) / 105.0, 0.0, 1.0);
        const double travel = (magnitude - 0.10) / 0.90;
        return std::copysign(std::pow(travel, 2.05) * (1.0 - 0.28 * speed), input);
    }
    double update(double input, double seconds, double kmh = 0) {
        const double target = curve(input, kmh);
        const bool unwind = value * target < 0 || std::abs(target) < std::abs(value);
        const double alpha = -std::expm1(-std::clamp(seconds, 0.0, 0.1) / (unwind ? 0.045 : 0.100));
        value += alpha * (target - value);
        if (target == 0 && std::abs(value) < 1.0 / 1024.0) value = 0;
        return value;
    }
    std::uint8_t axis(double input, double seconds, double kmh = 0) {
        const double v = update(input, seconds, kmh);
        return static_cast<std::uint8_t>(std::clamp(std::lround(128.0 + v * (v < 0 ? 128.0 : 127.0)), 0L, 255L));
    }
};
}
