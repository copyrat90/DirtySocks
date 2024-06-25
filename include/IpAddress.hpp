#pragma once

#include "platform_socket.hpp"
#include "platform_unicode.hpp"

#include <cstdint>
#include <optional>
#include <string>

namespace ds
{

// Only supports IPv4 for now
class IpAddress
{
public:
    IpAddress(std::uint8_t octet1, std::uint8_t octet2, std::uint8_t octet3, std::uint8_t octet4, std::uint16_t port);
    IpAddress(std::uint32_t address, std::uint16_t port);

    IpAddress(const sockaddr& addr);

    static auto resolve(const string_t& host, const string_t& service) -> std::optional<IpAddress>;

    /// @param host_and_port colon-seperated host & port string (e.g. `192.168.0.1:7878`)
    static auto resolve(string_view_t host_and_port) -> std::optional<IpAddress>;

public:
    auto get_presentation() const -> string_t;

private:
    sockaddr _addr;
};

} // namespace ds
