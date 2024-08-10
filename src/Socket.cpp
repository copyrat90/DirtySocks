#include "Socket.hpp"

#include "SocketAddress.hpp"

#include "System.hpp"

namespace ds
{

Socket::~Socket()
{
    close();
}

Socket::Socket(Socket&& other) noexcept : _handle(other._handle), _blocking(other._blocking)
{
    other._handle = INVALID_SOCKET;
    other._blocking = false;
}

Socket& Socket::operator=(Socket&& other) noexcept
{
    close();

    _handle = other._handle;
    other._handle = INVALID_SOCKET;

    _blocking = other._blocking;
    other._blocking = false;

    return *this;
}

void Socket::close()
{
    if (INVALID_SOCKET != _handle)
    {
#ifdef _WIN32
        closesocket(_handle);
#else // POSIX
        ::close(_handle);
#endif
        _handle = INVALID_SOCKET;
    }
}

void Socket::set_blocking(bool blocking)
{
    if (blocking != _blocking)
    {
        int nonblock_flag_set_result;
#ifdef _WIN32
        u_long enabled = blocking;
        nonblock_flag_set_result = ioctlsocket(_handle, FIONBIO, &enabled);
#else // POSIX
        auto flags = fcntl(_handle, F_GETFL, 0);
        if (SOCKET_ERROR == flags)
            nonblock_flag_set_result = SOCKET_ERROR;
        else
        {
            flags = blocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
            nonblock_flag_set_result = fcntl(_handle, F_SETFL, flags);
        }
#endif
        if (SOCKET_ERROR == nonblock_flag_set_result)
            DS_PRINT_ERROR("Socket::set_blocking() failed");
        else
            _blocking = blocking;
    }
}

bool Socket::is_blocking() const
{
    return _blocking;
}

Socket::Socket(SOCKET handle, bool blocking) : _handle(handle), _blocking(blocking)
{
}

auto Socket::get_handle() const -> SOCKET
{
    return _handle;
}

void Socket::init_handle(IpVersion ip_ver, Protocol protocol)
{
    close();

    const auto family = (IpVersion::V6 == ip_ver ? AF_INET6 : AF_INET);
    const auto type = (Protocol::UDP == protocol ? SOCK_DGRAM : SOCK_STREAM);

    _handle = ::socket(family, type, 0);
}

} // namespace ds
