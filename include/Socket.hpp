#pragma once

#include "platform_socket.hpp"

#include "IpVersion.hpp"

namespace ds
{

class SocketAddress;

class Socket
{
public:
    enum class Result
    {
        DONE,
        NOT_READY,
        PARTIAL,
        DISCONNECTED,
        UNEXPECTED_ERROR,
    };

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
    void set_blocking(bool blocking);
    bool is_blocking() const;

protected:
    enum class Protocol
    {
        TCP,
        UDP,
    };

protected:
    Socket(SOCKET, bool blocking);

    auto get_handle() const -> SOCKET;

    void init_handle(IpVersion, Protocol);

private:
    SOCKET _handle = INVALID_SOCKET;
    bool _blocking = false;
};

} // namespace ds
