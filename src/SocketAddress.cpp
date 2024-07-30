#include "SocketAddress.hpp"

#include "System.hpp"

#include <format>
#include <stdexcept>

namespace ds
{

auto SocketAddress::resolve(string_view_t host, string_view_t service,
                            IpVersion ip_version) -> std::optional<SocketAddress>
{
    std::optional<SocketAddress> result;

#if defined(_WIN32) && defined(_UNICODE)
    using addrinfo_t = ADDRINFOW;
#else
    using addrinfo_t = addrinfo;
#endif

    addrinfo_t* addr_list = nullptr;
    addrinfo_t hints{};

    if (ip_version == IpVersion::V4)
        hints.ai_family = AF_INET;
    else if (ip_version == IpVersion::V6)
        hints.ai_family = AF_INET6;
    else // both IPv4 & IPv6
        hints.ai_family = AF_UNSPEC;

#if defined(_WIN32) && defined(_UNICODE)
    const auto error = GetAddrInfoW(host.data(), service.data(), &hints, &addr_list);
#else
    const auto error = getaddrinfo(host.data(), service.data(), &hints, &addr_list);
#endif

    if (error != 0)
    {
        DS_PRINT_ERROR("SocketAddress::resolve() failed");
    }
    else
    {
        // find item that has proper `ai_addr`
        for (const addrinfo_t* cur = addr_list; cur != nullptr; cur = cur->ai_next)
        {
            if (cur->ai_addr)
            {
                result = SocketAddress(*cur->ai_addr);
                break;
            }
        }
    }

    if (addr_list)
#if defined(_WIN32) && defined(_UNICODE)
        FreeAddrInfoW(addr_list);
#else
        freeaddrinfo(addr_list);
#endif

    return result;
}

auto SocketAddress::get_presentation() const -> string_t
{
    std::conditional_t<WIN32_UNICODE, wchar_t, char> str_buf[INET6_ADDRSTRLEN + 1];

    const void* addr = nullptr;
    if (_addr.ss_family == AF_INET)
        addr = &reinterpret_cast<const sockaddr_in&>(_addr).sin_addr;
    else if (_addr.ss_family == AF_INET6)
        addr = &reinterpret_cast<const sockaddr_in6&>(_addr).sin6_addr;
    else
        throw std::logic_error(std::format("Invalid ss_family=", _addr.ss_family));

#if defined(_WIN32) && defined(_UNICODE)
    if (!InetNtopW(_addr.ss_family, addr, str_buf, sizeof(str_buf)))
#else
    if (!inet_ntop(_addr.ss_family, addr, str_buf, sizeof(str_buf)))
#endif
    {
        DS_PRINT_ERROR("SocketAddress::get_presentation() failed");
        return string_t();
    }

    if (_addr.ss_family == AF_INET)
        return std::format(TEXT("{}:{}"), str_buf, ntohi(reinterpret_cast<const sockaddr_in&>(_addr).sin_port));
    // AF_INET6
    return std::format(TEXT("[{}]:{}"), str_buf, ntohi(reinterpret_cast<const sockaddr_in6&>(_addr).sin6_port));
}

SocketAddress::SocketAddress(const sockaddr& addr)
{
    if (addr.sa_family == AF_INET)
        reinterpret_cast<sockaddr_in&>(_addr) = reinterpret_cast<const sockaddr_in&>(addr);
    else if (addr.sa_family == AF_INET6)
        reinterpret_cast<sockaddr_in6&>(_addr) = reinterpret_cast<const sockaddr_in6&>(addr);
    else
        throw std::logic_error(std::format("Invalid sa_family=", addr.sa_family));
}

} // namespace ds
