#include "DirtySocks/SocketAddress.hpp"

#include "DirtySocks/System.hpp"

#ifndef _WIN32 // POSIX
#include "DirtySocks/ErrorCodes.hpp"
#endif

#include <format>

namespace ds
{

SocketAddress::SocketAddress(std::uint8_t octet1, std::uint8_t octet2, std::uint8_t octet3, std::uint8_t octet4,
                             std::uint16_t port) noexcept
    : SocketAddress(static_cast<std::uint32_t>(octet1 << 24u | octet2 << 16u | octet3 << 8u | octet4), port)
{
}

SocketAddress::SocketAddress(std::uint32_t ip, std::uint16_t port) noexcept
{
    auto& addr = reinterpret_cast<sockaddr_in&>(_addr);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(ip);
    addr.sin_port = htons(port);
}

SocketAddress::SocketAddress(const sockaddr& addr)
{
    if (addr.sa_family == AF_INET6)
        reinterpret_cast<sockaddr_in6&>(_addr) = reinterpret_cast<const sockaddr_in6&>(addr);
    else // AF_INET
        reinterpret_cast<sockaddr_in&>(_addr) = reinterpret_cast<const sockaddr_in&>(addr);
}

SocketAddress::SocketAddress(const SocketAddress& other)
{
    operator=(other);
}

SocketAddress& SocketAddress::operator=(const SocketAddress& other)
{
    if (other._addr.ss_family == AF_INET6)
        reinterpret_cast<sockaddr_in6&>(_addr) = reinterpret_cast<const sockaddr_in6&>(other._addr);
    else // AF_INET
        reinterpret_cast<sockaddr_in&>(_addr) = reinterpret_cast<const sockaddr_in&>(other._addr);

    return *this;
}

auto SocketAddress::resolve(string_view_t host, string_view_t service, IpVersion ip_version,
                            std::error_code& ec) -> std::optional<SocketAddress>
{
    ec.clear();

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
#ifdef _WIN32
        ec = System::get_last_error_code();
#else // POSIX
        ec = static_cast<AddrInfoErrc>(error);
#endif
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

auto SocketAddress::any(std::uint16_t port, IpVersion ip_version) -> SocketAddress
{
    sockaddr_storage storage{};

    if (IpVersion::V6 == ip_version)
    {
        auto& addr = reinterpret_cast<sockaddr_in6&>(storage);
        addr.sin6_family = AF_INET6;
        addr.sin6_addr = in6addr_any;
        addr.sin6_port = htons(port);
    }
    else
    {
        auto& addr = reinterpret_cast<sockaddr_in&>(storage);
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);
    }

    return SocketAddress(reinterpret_cast<sockaddr&>(storage));
}

auto SocketAddress::get_presentation(std::error_code& ec) const -> string_t
{
    ec.clear();

    std::conditional_t<WIN32_UNICODE, wchar_t, char> str_buf[INET6_ADDRSTRLEN + 1];

    const void* addr = nullptr;
    if (_addr.ss_family == AF_INET6)
        addr = &reinterpret_cast<const sockaddr_in6&>(_addr).sin6_addr;
    else // AF_INET
        addr = &reinterpret_cast<const sockaddr_in&>(_addr).sin_addr;

#if defined(_WIN32) && defined(_UNICODE)
    if (!InetNtopW(_addr.ss_family, addr, str_buf, sizeof(str_buf)))
#else
    if (!inet_ntop(_addr.ss_family, addr, str_buf, sizeof(str_buf)))
#endif
    {
        ec = System::get_last_error_code();
        return string_t();
    }

    if (_addr.ss_family == AF_INET)
        return std::format(TEXT("{}:{}"), str_buf, ntohs(reinterpret_cast<const sockaddr_in&>(_addr).sin_port));
    // AF_INET6
    return std::format(TEXT("[{}]:{}"), str_buf, ntohs(reinterpret_cast<const sockaddr_in6&>(_addr).sin6_port));
}

auto SocketAddress::get_port() const -> std::uint16_t
{
    if (_addr.ss_family == AF_INET)
        return ntohs(reinterpret_cast<const sockaddr_in&>(_addr).sin_port);
    // AF_INET6
    return ntohs(reinterpret_cast<const sockaddr_in6&>(_addr).sin6_port);
}

auto SocketAddress::get_ip_version() const -> IpVersion
{
    if (_addr.ss_family == AF_INET6)
        return IpVersion::V6;
    else
        return IpVersion::V4;
}

auto SocketAddress::get_sockaddr() const -> const sockaddr&
{
    return reinterpret_cast<const sockaddr&>(_addr);
}

auto SocketAddress::get_sockaddr_len() const -> socklen_t
{
    return static_cast<socklen_t>(sizeof(_addr));
}

} // namespace ds
