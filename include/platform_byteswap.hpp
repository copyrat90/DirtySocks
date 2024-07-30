#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <concepts>
#include <cstddef>

namespace ds
{

static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big,
              "Unknown native endian");

template <std::integral Int>
constexpr Int byteswap(Int value) noexcept
{
    auto byte_array = std::bit_cast<std::array<std::byte, sizeof(Int)>>(value);
    std::ranges::reverse(byte_array);
    return std::bit_cast<Int>(byte_array);
}

template <std::integral Int>
constexpr Int htoni(Int value) noexcept
{
    if constexpr (std::endian::native == std::endian::little)
    {
        value = byteswap(value);
    }

    return value;
}

template <std::integral Int>
constexpr Int ntohi(Int value) noexcept
{
    return htoni<Int>(value);
}

} // namespace ds
