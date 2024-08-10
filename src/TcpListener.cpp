#include "TcpListener.hpp"

#include "SocketAddress.hpp"
#include "System.hpp"
#include "TcpSocket.hpp"

namespace ds
{

auto TcpListener::listen(const SocketAddress& addr) -> Result
{
    init_handle(addr.get_ip_version(), Socket::Protocol::TCP);

    if (SOCKET_ERROR == ::listen(get_handle(), SOMAXCONN))
    {
        DS_PRINT_ERROR("TcpListener::listen() failed");
        return Result::UNEXPECTED_ERROR;
    }

    return Result::DONE;
}

auto TcpListener::accept(TcpSocket& socket) -> Result
{
    SOCKET handle = ::accept(get_handle(), nullptr, nullptr);

    if (INVALID_SOCKET == handle)
    {
#ifdef _WIN32
        if (WSAEWOULDBLOCK == WSAGetLastError())
#else // POSIX
        if (EWOULDBLOCK == errno || EAGAIN == errno)
#endif
            return Result::NOT_READY;
        else
        {
            DS_PRINT_ERROR("TcpListener::accept() failed");
            return Result::UNEXPECTED_ERROR;
        }
    }

    socket = TcpSocket(handle, is_blocking());

    return Result::DONE;
}

} // namespace ds
