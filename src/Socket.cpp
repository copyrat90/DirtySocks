#include "Socket.hpp"

#include "SocketAddress.hpp"

#include "System.hpp"

namespace ds
{

Socket::~Socket()
{
    close();
}

Socket::Socket(Socket&& other) noexcept : _handle(other._handle), _non_blocking(other._non_blocking)
{
    other._handle = INVALID_SOCKET;
    other._non_blocking = false;
}

Socket& Socket::operator=(Socket&& other) noexcept
{
    close();

    _handle = other._handle;
    other._handle = INVALID_SOCKET;

    _non_blocking = other._non_blocking;
    other._non_blocking = false;

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

void Socket::set_non_blocking(bool non_blocking, std::error_code& ec)
{
    ec.clear();

    int nonblock_flag_set_result;
#ifdef _WIN32
    u_long enabled = non_blocking;
    nonblock_flag_set_result = ioctlsocket(_handle, FIONBIO, &enabled);
#else // POSIX
    auto flags = fcntl(_handle, F_GETFL, 0);
    if (SOCKET_ERROR == flags)
        nonblock_flag_set_result = SOCKET_ERROR;
    else
    {
        flags = non_blocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
        nonblock_flag_set_result = fcntl(_handle, F_SETFL, flags);
    }
#endif
    if (SOCKET_ERROR == nonblock_flag_set_result)
        ec = System::get_last_error_code();
    else
        _non_blocking = non_blocking;
}

bool Socket::is_non_blocking() const
{
    return _non_blocking;
}

auto Socket::get_handle() const -> SOCKET
{
    return _handle;
}

Socket::Socket(SOCKET handle, bool non_blocking) : _handle(handle), _non_blocking(non_blocking)
{
}

void Socket::init_handle(IpVersion ip_ver, Protocol protocol, std::error_code& ec)
{
    close();

    const auto family = (IpVersion::V6 == ip_ver ? AF_INET6 : AF_INET);
    const auto type = (Protocol::UDP == protocol ? SOCK_DGRAM : SOCK_STREAM);

    _handle = ::socket(family, type, 0);

    if (!is_non_blocking())
        set_non_blocking(false, ec);
}

} // namespace ds
