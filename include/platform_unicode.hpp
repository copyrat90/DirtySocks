#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <type_traits>

#include <string>
#include <string_view>

namespace ds
{

#if defined(_WIN32) && defined(_UNICODE)
inline constexpr bool WIN32_UNICODE = true;
#else
inline constexpr bool WIN32_UNICODE = false;
#endif

#ifndef TEXT
#define TEXT(str) str
#endif

using string_t = std::conditional_t<WIN32_UNICODE, std::wstring, std::string>;
using string_view_t = std::conditional_t<WIN32_UNICODE, std::wstring_view, std::string_view>;
using error_stream_t = std::conditional_t<WIN32_UNICODE, std::wostream, std::ostream>;

} // namespace ds
