#include "TcpListener.hpp"

#include "SocketAddress.hpp"
#include "System.hpp"
#include "TcpSocket.hpp"

namespace ds
{

auto TcpListener::listen(const SocketAddress& addr) -> Result
{
    init_handle(addr.get_ip_version(), Socket::Protocol::TCP);

    if (SOCKET_ERROR == ::bind(get_handle(), &addr.get_sockaddr(), addr.get_sockaddr_len()))
    {
        DS_PRINT_ERROR("TcpListener::listen() - ::bind() failed");
        return Result::ERROR;
    }

    if (SOCKET_ERROR == ::listen(get_handle(), SOMAXCONN))
    {
        DS_PRINT_ERROR("TcpListener::listen() - ::listen() failed");
        return Result::ERROR;
    }

    return Result::DONE;
}

auto TcpListener::accept(TcpSocket& out_socket) -> Result
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
            return Result::ERROR;
        }
    }

    out_socket = TcpSocket(handle, is_blocking());

    // inherit non-blocking option manually
    // (on some platforms, client socket doesn't inherit non-blocking option from listener socket)
    out_socket.set_blocking(is_blocking());

    return Result::DONE;
}

} // namespace ds
