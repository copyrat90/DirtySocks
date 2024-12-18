#pragma once

#include "DirtySocks/PlatformSocket.hpp"
#include "DirtySocks/PlatformUnicode.hpp"

#include "DirtySocks/IpVersion.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <system_error>

namespace ds
{

/// @brief BSD socket `sockaddr` abstraction (IP & port)
///
/// This class supports both IPv4 & IPv6.
/// Use port number `0` for a dynamic port.
class SocketAddress final
{
public:
    /// @brief Construct an IPv4 socket address (`sockaddr_in`) with IP & port numbers
    /// @param octet1 IP octet 1 (e.g. `192`)
    /// @param octet2 IP octet 2 (e.g. `168`)
    /// @param octet3 IP octet 3 (e.g. `0`)
    /// @param octet4 IP octet 4 (e.g. `1`)
    /// @param port port number (use `0` for a dynamic port)
    SocketAddress(std::uint8_t octet1, std::uint8_t octet2, std::uint8_t octet3, std::uint8_t octet4,
                  std::uint16_t port) noexcept;

    /// @brief Construct an IPv4 socket address (`sockaddr_in`) with IP & port numbers
    SocketAddress(std::uint32_t ip, std::uint16_t port) noexcept;

    /// @brief Construct a socket address with `sockaddr` directly
    SocketAddress(const sockaddr& addr);

    /// @brief Copy constructor
    SocketAddress(const SocketAddress&);

    /// @brief Copy assignment operator
    SocketAddress& operator=(const SocketAddress&);

public:
    /// @brief Resolve the host with `getaddrinfo()`.
    /// @param host null-terminated host name (e.g. `www.example.com`, `192.168.0.1`, `localhost`, `::1`, ...)
    /// @param service null-terminated service name (e.g. `http`, `80`, ...)
    /// @param ip_version IP protocol version to restrict to
    /// @return socket address if resolved properly, otherwise `std::nullopt`
    static auto resolve(string_view_t host, string_view_t service, IpVersion ip_version,
                        std::error_code&) -> std::optional<SocketAddress>;

    /// @brief Get the any address used for binding (`INADDR_ANY` or `inaddr6_any`).
    ///
    /// This function doesn't have error code parameter.
    /// If you give an invalid `ip_version`, this will just return IPv4 any address (`INADDR_ANY`)
    ///
    /// @param ip_version IP protocol version to get the any address
    /// @return any address for given `ip_version`
    static auto any(std::uint16_t port, IpVersion ip_version) -> SocketAddress;

public:
    /// @brief Get presentation string, including both IP and port.
    auto get_presentation(std::error_code&) const -> string_t;

    auto get_port() const -> std::uint16_t;

    auto get_ip_version() const -> IpVersion;

    auto get_sockaddr() const -> const sockaddr&;
    auto get_sockaddr_len() const -> socklen_t;

private:
    sockaddr_storage _addr;
};

} // namespace ds
