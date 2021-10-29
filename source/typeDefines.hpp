// typeDefines.hpp
// Common engine defines
#pragma once
#include <cstdint>
#include <numbers>

namespace fe
    {
        using int64 = std::int64_t;
        using uint64 = std::uint64_t;

        using str = std::uint64_t;
        using index = std::uint32_t;

        constexpr static double EPSILON = 0.0001;
        constexpr static double PI = std::numbers::pi;
    }
