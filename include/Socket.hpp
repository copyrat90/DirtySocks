#pragma once

#include "platform_socket.hpp"

#include "IpVersion.hpp"

#include <system_error>

namespace ds
{

class SocketAddress;

class Socket
{
public:
    virtual ~Socket() = 0;

    Socket() = default;

public:
    Socket(Socket&&) noexcept;
    Socket& operator=(Socket&&) noexcept;

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

public:
    void close();

public:
    void set_non_blocking(bool non_blocking, std::error_code&);
    bool is_non_blocking() const;

public:
    auto get_handle() const -> SOCKET;

protected:
    enum class Protocol
    {
        TCP,
        UDP,
    };

protected:
    Socket(SOCKET, bool non_blocking);

    void init_handle(IpVersion, Protocol, std::error_code&);

private:
    SOCKET _handle = INVALID_SOCKET;
    bool _non_blocking = false;
};

} // namespace ds
