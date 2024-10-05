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
        IN_PROGRESS,
        DISCONNECTED,
        ERROR,
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
    void set_non_blocking(bool non_blocking);
    bool is_non_blocking() const;

protected:
    enum class Protocol
    {
        TCP,
        UDP,
    };

protected:
    Socket(SOCKET, bool non_blocking);

    auto get_handle() const -> SOCKET;

    void init_handle(IpVersion, Protocol);

protected:
    auto get_result_from_error() const -> Result;

private:
    SOCKET _handle = INVALID_SOCKET;
    bool _non_blocking = false;
};

} // namespace ds
