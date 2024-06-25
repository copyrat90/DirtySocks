#include "IpAddress.hpp"

#include <cassert>
#include <format>

#include "System.hpp"

namespace ds
{

IpAddress::IpAddress(std::uint8_t octet1, std::uint8_t octet2, std::uint8_t octet3, std::uint8_t octet4,
                     std::uint16_t port)
    : IpAddress(static_cast<std::uint32_t>(octet1 << 24u | octet2 << 16u | octet3 << 8u | octet4), port)
{
}

IpAddress::IpAddress(std::uint32_t address, std::uint16_t port)
    : _addr{} // zero-initialization is necessary for some unused bytes
{
    auto& addr = reinterpret_cast<sockaddr_in&>(_addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
#ifdef _WIN32
    addr.sin_addr.S_un.S_addr = htonl(address);
#else // POSIX
    addr.sin_addr.s_addr = htonl(address);
#endif
}

IpAddress::IpAddress(const sockaddr& addr) : _addr(addr)
{
}

auto IpAddress::resolve(const string_t& host, const string_t& service) -> std::optional<IpAddress>
{
    std::optional<IpAddress> result;

#if defined(_WIN32) && defined(_UNICODE)
    using addrinfo_t = ADDRINFOW;
#else
    using addrinfo_t = addrinfo;
#endif
    addrinfo_t* addr_list = nullptr;
    addrinfo_t hints{};
    hints.ai_family = AF_INET;
#if defined(_WIN32) && defined(_UNICODE)
    const auto error = GetAddrInfoW(host.c_str(), service.c_str(), &hints, &addr_list);
#else
    const auto error = getaddrinfo(host.c_str(), service.c_str(), &hints, &addr_list);
#endif

    if (error != 0)
    {
        DS_PRINT_ERROR("IpAddress::resolve() failed");
    }
    else
    {
        // find item that has proper `ai_addr`
        for (addrinfo_t* ptr = addr_list; ptr != nullptr; ptr = ptr->ai_next)
        {
            if (ptr->ai_addr)
            {
                result.emplace(*ptr->ai_addr);
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

auto IpAddress::resolve(string_view_t host_and_port) -> std::optional<IpAddress>
{
    const auto pos = host_and_port.find_last_of(TEXT(":"));
    string_t host, service;
    if (pos != string_view_t::npos)
    {
        host = host_and_port.substr(0, pos);
        service = host_and_port.substr(pos + 1);
    }
    else
    {
        host = host_and_port;
        service = TEXT("0"); // use default port...
    }

    return resolve(host, service);
}

auto IpAddress::get_presentation() const -> string_t
{
    std::conditional_t<WIN32_UNICODE, wchar_t, char> str_buf[INET_ADDRSTRLEN];

#if defined(_WIN32) && defined(_UNICODE)
    InetNtopW(_addr.sa_family, &reinterpret_cast<const sockaddr_in&>(_addr).sin_addr, str_buf, sizeof(str_buf));
#else
    inet_ntop(_addr.sa_family, &reinterpret_cast<const sockaddr_in&>(_addr).sin_addr, str_buf, sizeof(str_buf));
#endif

    return std::format(TEXT("{}:{}"), str_buf, ntohs(reinterpret_cast<const sockaddr_in&>(_addr).sin_port));
}

} // namespace ds
