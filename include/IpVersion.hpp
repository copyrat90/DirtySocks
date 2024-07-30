#pragma once

#include "enum_as_flags.hpp"

namespace ds
{

enum class IpVersion
{
    NONE = 0,
    V4 = (1 << 0),
    V6 = (1 << 1),
    BOTH = V4 | V6
};

ENUM_AS_FLAGS(IpVersion);

} // namespace ds
