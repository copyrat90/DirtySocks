#include "SocketSelector.hpp"

#include "ErrorCodes.hpp"
#include "Socket.hpp"
#include "System.hpp"

#ifndef _WIN32 // POSIX
#include <algorithm>
#endif

namespace ds
{

int SocketSelector::select(timeval* timeout, std::error_code& ec)
{
    ec.clear();

    _read_set.result = _read_set.all;
    _write_set.result = _write_set.all;
    _except_set.result = _except_set.all;

#ifdef _WIN32
    const int nfds = 0;
#else // POSIX
    const int nfds = 1 + std::max({_read_set.max_fd, _write_set.max_fd, _except_set.max_fd});
#endif

    const int select_result = ::select(nfds, ((0 == _read_set.sockets_count) ? nullptr : &_read_set.result),
                                       ((0 == _write_set.sockets_count) ? nullptr : &_write_set.result),
                                       ((0 == _except_set.sockets_count) ? nullptr : &_except_set.result), timeout);
    if (SOCKET_ERROR == select_result)
    {
        ec = System::get_last_error_code();
        return 0;
    }

    return select_result;
}

bool SocketSelector::has_read(const Socket& sock) const
{
    return _read_set.has(sock);
}

bool SocketSelector::has_write(const Socket& sock) const
{
    return _write_set.has(sock);
}

bool SocketSelector::has_except(const Socket& sock) const
{
    return _except_set.has(sock);
}

void SocketSelector::add_to_read_set(const Socket& sock, std::error_code& ec)
{
    _read_set.add(sock, ec);
}

void SocketSelector::add_to_write_set(const Socket& sock, std::error_code& ec)
{
    _write_set.add(sock, ec);
}

void SocketSelector::add_to_except_set(const Socket& sock, std::error_code& ec)
{
    _except_set.add(sock, ec);
}

void SocketSelector::remove_from_read_set(const Socket& sock)
{
    _read_set.remove(sock);
}

void SocketSelector::remove_from_write_set(const Socket& sock)
{
    _write_set.remove(sock);
}

void SocketSelector::remove_from_except_set(const Socket& sock)
{
    _except_set.remove(sock);
}

void SocketSelector::clear_read_set()
{
    _read_set.clear();
}

void SocketSelector::clear_write_set()
{
    _write_set.clear();
}

void SocketSelector::clear_except_set()
{
    _except_set.clear();
}

SocketSelector::Set::Set()
{
    FD_ZERO(&all);
    FD_ZERO(&result);
}

bool SocketSelector::Set::has(const Socket& sock) const
{
    return FD_ISSET(sock.get_handle(), &result);
}

void SocketSelector::Set::add(const Socket& sock, std::error_code& ec)
{
    ec.clear();

    // do nothing if duplicate passed
    if (FD_ISSET(sock.get_handle(), &all))
        return;

    if (sockets_count >= FD_SETSIZE)
    {
        ec = SocketSelectorErrc::FD_COUNT_TOO_MANY;
        return;
    }

#ifndef _WIN32 // POSIX
    if (sock.get_handle() >= FD_SETSIZE)
    {
        ec = SocketSelectorErrc::FD_VALUE_TOO_BIG;
        return;
    }

    max_fd = std::max(max_fd, sock.get_handle());
#endif
    FD_SET(sock.get_handle(), &all);
    ++sockets_count;
}

void SocketSelector::Set::remove(const Socket& sock)
{
    // do nothing if non-existing socket passed
    if (!FD_ISSET(sock.get_handle(), &all))
        return;

    FD_CLR(sock.get_handle(), &all);
    --sockets_count;
}

void SocketSelector::Set::clear()
{
    FD_ZERO(&all);
    sockets_count = 0;
#ifndef _WIN32 // POSIX
    max_fd = INVALID_SOCKET;
#endif
}

} // namespace ds
