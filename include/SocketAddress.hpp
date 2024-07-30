#pragma once

#include "platform_byteswap.hpp"
#include "platform_socket.hpp"
#include "platform_unicode.hpp"

#include "IpVersion.hpp"

#include <cstdint>
#include <optional>
#include <string>

namespace ds
{

/// @brief BSD socket `sockaddr` abstraction (IP & port)
///
/// This class supports both IPv4 & IPv6.
/// Use port number `0` for a dynamic port.
class SocketAddress
{
public:
    /// @brief Resolve the host with `getaddrinfo()`.
    /// @param host null-terminated host name (e.g. `www.example.com`, `192.168.0.1`, `localhost`, `::1`, ...)
    /// @param service null-terminated service name (e.g. `http`, `80`, ...)
    /// @param ip_version IP protocol version to restrict to
    /// @return socket address if resolved properly, otherwise `std::nullopt`
    static auto resolve(string_view_t host, string_view_t service,
                        IpVersion ip_version) -> std::optional<SocketAddress>;

public:
    /// @brief Get presentation string, including both IP and port.
    auto get_presentation() const -> string_t;

public:
    /// @brief Construct an IPv4 socket address (`sockaddr_in`) with IP & port numbers
    /// @param octet1 IP octet 1 (e.g. `192`)
    /// @param octet2 IP octet 2 (e.g. `168`)
    /// @param octet3 IP octet 3 (e.g. `0`)
    /// @param octet4 IP octet 4 (e.g. `1`)
    /// @param port port number (use `0` for a dynamic port)
    constexpr SocketAddress(std::uint8_t octet1, std::uint8_t octet2, std::uint8_t octet3, std::uint8_t octet4,
                            std::uint16_t port) noexcept
        : SocketAddress(static_cast<std::uint32_t>(octet1 << 24u | octet2 << 16u | octet3 << 8u | octet4), port)
    {
    }

    /// @brief Construct an IPv4 socket address (`sockaddr_in`) with IP & port numbers
    constexpr SocketAddress(std::uint32_t ip, std::uint16_t port) noexcept
    {
        // MSVC doesn't like `reinterpret_cast` in constexpr context
        auto& addr = (sockaddr_in&)(_addr);

        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htoni(ip);
        addr.sin_port = htoni(port);
    }

private:
    /// @brief Construct a socket address with `sockaddr` directly
    SocketAddress(const sockaddr& addr);

private:
    sockaddr_storage _addr;
};

} // namespace ds
