// Rudy Castan
#pragma once
#include <algorithm>

namespace math
{
    // Linearly interpolate between any start and stop values based off of an interpolant, where the interpolant
    // is assumed to be within the range [0,1].
    [[nodiscard]] constexpr double lerp(double interpolant, double start, double stop) noexcept
    {
        return start + interpolant * (stop - start);
    }

    [[nodiscard]] constexpr double map_linearly(double input, double input_start, double input_stop,
                                                double output_start, double output_stop) noexcept
    {
        const double t = (input - input_start) / (input_stop - input_start);
        return lerp(t, output_start, output_stop);
    }

    [[nodiscard]] constexpr double map_linearly_and_clamp(double input, double input_start, double input_stop,
                                                          double output_start, double output_stop) noexcept
    {
        const auto result = map_linearly(input, input_start, input_stop, output_start, output_stop);
        const auto low    = std::min(output_start, output_stop);
        const auto high   = std::max(output_start, output_stop);
        return std::clamp(result, low, high);
    }
}
