#include "ErrorCodes.hpp"

#ifdef _WIN32
#include <Windows.h>
#else // POSIX
#include <string.h>
#endif

namespace ds
{

namespace
{

static constexpr int MAX_ERR_MSG_LEN = 256;

class AddrInfoErrorCategory : public std::error_category
{
public:
    static auto instance() -> AddrInfoErrorCategory&
    {
        static AddrInfoErrorCategory category;
        return category;
    }

public:
    auto name() const noexcept -> const char* override
    {
        return "DirtySocks::AddrInfoError";
    }

    auto message(int error_value) const -> std::string override
    {
#ifdef _WIN32
        char result[MAX_ERR_MSG_LEN] = {"(Invalid error message)"};
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error_value,
                       MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), result, MAX_ERR_MSG_LEN, nullptr);
#else
        const char* result = gai_strerror(error_value);
#endif
        return result;
    }

private:
    AddrInfoErrorCategory() = default;
};

class SystemErrorCategory : public std::error_category
{
public:
    static auto instance() -> SystemErrorCategory&
    {
        static SystemErrorCategory category;
        return category;
    }

public:
    auto name() const noexcept -> const char* override
    {
        return "DirtySocks::SystemError";
    }

    auto message(int error_value) const -> std::string override
    {
        char buf[MAX_ERR_MSG_LEN] = {"(Invalid error message)"};
        char* result = buf;
#ifdef _WIN32
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error_value,
                       MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), result, MAX_ERR_MSG_LEN, nullptr);
#else // POSIX
        result = xsi_or_gnu_strerror_r(strerror_r(error_value, buf, sizeof(buf)), result);
#endif

        return result;
    }

#ifndef _WIN32 // POSIX
private:
    // xsi
    char* xsi_or_gnu_strerror_r(int, char* result) const
    {
        return result;
    }

    // gnu
    char* xsi_or_gnu_strerror_r(char* result, char*) const
    {
        return result;
    }
#endif

private:
    SystemErrorCategory() = default;
};

} // namespace

auto make_error_code(AddrInfoErrc errc) -> std::error_code
{
    return std::error_code(static_cast<int>(errc), AddrInfoErrorCategory::instance());
}

auto make_error_code(SystemErrc errc) -> std::error_code
{
    return std::error_code(static_cast<int>(errc), SystemErrorCategory::instance());
}

} // namespace ds