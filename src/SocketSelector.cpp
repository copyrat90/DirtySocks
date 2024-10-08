#include "SocketSelector.hpp"

#include "Socket.hpp"
#include "System.hpp"

#ifndef _WIN32 // POSIX
#include <algorithm>
#include <ranges>
#endif

namespace ds
{

SocketSelector::SocketSelector()
{
    FD_ZERO(&_read_set);
    FD_ZERO(&_write_set);
    FD_ZERO(&_except_set);

    _read_sockets.reserve(FD_SETSIZE);
    _write_sockets.reserve(FD_SETSIZE);
    _except_sockets.reserve(FD_SETSIZE);
}

void SocketSelector::select(timeval* timeout, std::error_code& ec)
{
    ec.clear();

    FD_ZERO(&_read_set);
    FD_ZERO(&_write_set);
    FD_ZERO(&_except_set);

    for (const Socket* fd : _read_sockets)
        FD_SET(fd->get_handle(), &_read_set);
    for (const Socket* fd : _write_sockets)
        FD_SET(fd->get_handle(), &_read_set);
    for (const Socket* fd : _except_sockets)
        FD_SET(fd->get_handle(), &_except_set);

#ifdef _WIN32
    const int nfds = 0;
#else // POSIX
    const int nfds = 1 + std::max(get_max_read_fd(), get_max_write_fd());
#endif

    if (SOCKET_ERROR == ::select(nfds, (_read_sockets.empty() ? nullptr : &_read_set),
                                 (_write_sockets.empty() ? nullptr : &_write_set),
                                 (_except_sockets.empty() ? nullptr : &_except_set), timeout))
    {
        ec = System::get_last_error_code();
        return;
    }
}

bool SocketSelector::has_read(const Socket& sock) const
{
    return FD_ISSET(sock.get_handle(), &_read_set);
}

bool SocketSelector::has_write(const Socket& sock) const
{
    return FD_ISSET(sock.get_handle(), &_write_set);
}

bool SocketSelector::has_except(const Socket& sock) const
{
    return FD_ISSET(sock.get_handle(), &_except_set);
}

void SocketSelector::add_to_read_set(const Socket& sock)
{
#ifndef _WIN32 // POSIX
    if (_max_read_fd)
        _max_read_fd = std::max(*_max_read_fd, sock.get_handle());
#endif

    _read_sockets.insert(&sock);
}

void SocketSelector::add_to_write_set(const Socket& sock)
{
#ifndef _WIN32 // POSIX
    if (_max_write_fd)
        _max_write_fd = std::max(*_max_write_fd, sock.get_handle());
#endif

    _write_sockets.insert(&sock);
}

void SocketSelector::add_to_except_set(const Socket& sock)
{
#ifndef _WIN32 // POSIX
    if (_max_except_fd)
        _max_except_fd = std::max(*_max_except_fd, sock.get_handle());
#endif

    _except_sockets.insert(&sock);
}

void SocketSelector::remove_from_read_set(const Socket& sock)
{
#ifndef _WIN32 // POSIX
    if (_max_read_fd && (*_max_read_fd == sock.get_handle()))
        _max_read_fd.reset();
#endif

    _read_sockets.erase(&sock);
}

void SocketSelector::remove_from_write_set(const Socket& sock)
{
#ifndef _WIN32 // POSIX
    if (_max_write_fd && (*_max_write_fd == sock.get_handle()))
        _max_write_fd.reset();
#endif

    _write_sockets.erase(&sock);
}

void SocketSelector::remove_from_except_set(const Socket& sock)
{
#ifndef _WIN32 // POSIX
    if (_max_except_fd && (*_max_except_fd == sock.get_handle()))
        _max_except_fd.reset();
#endif

    _except_sockets.erase(&sock);
}

void SocketSelector::clear_read_set()
{
#ifndef _WIN32 // POSIX
    _max_read_fd.reset();
#endif

    _read_sockets.clear();
}

void SocketSelector::clear_write_set()
{
#ifndef _WIN32 // POSIX
    _max_write_fd.reset();
#endif

    _write_sockets.clear();
}

void SocketSelector::clear_except_set()
{
#ifndef _WIN32 // POSIX
    _max_except_fd.reset();
#endif

    _except_sockets.clear();
}

#ifndef _WIN32 // POSIX

auto SocketSelector::get_max_read_fd() -> SOCKET
{
    if (_max_read_fd)
        return *_max_read_fd;

    if (_read_sockets.empty())
        return INVALID_SOCKET;

    auto fd_view = std::ranges::views::transform([](const Socket* sock) { return sock->get_handle(); });

    _max_read_fd = std::ranges::max(_read_sockets | fd_view);
    return *_max_read_fd;
}

auto SocketSelector::get_max_write_fd() -> SOCKET
{
    if (_max_write_fd)
        return *_max_write_fd;

    if (_write_sockets.empty())
        return INVALID_SOCKET;

    auto fd_view = std::ranges::views::transform([](const Socket* sock) { return sock->get_handle(); });

    _max_write_fd = std::ranges::max(_write_sockets | fd_view);
    return *_max_write_fd;
}

auto SocketSelector::get_max_except_fd() -> SOCKET
{
    if (_max_except_fd)
        return *_max_except_fd;

    if (_except_sockets.empty())
        return INVALID_SOCKET;

    auto fd_view = std::ranges::views::transform([](const Socket* sock) { return sock->get_handle(); });

    _max_except_fd = std::ranges::max(_except_sockets | fd_view);
    return *_max_except_fd;
}

#endif

} // namespace ds
