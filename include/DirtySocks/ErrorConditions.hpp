#pragma once

#include <system_error>
#include <type_traits>

namespace ds
{

enum class SocketErrc
{
    OK = 0,

    WOULD_BLOCK,
    DISCONNECTED,
};

auto make_error_condition(SocketErrc) -> std::error_condition;

} // namespace ds

namespace std
{

template <>
struct is_error_condition_enum<ds::SocketErrc> : true_type
{
};

} // namespace std
