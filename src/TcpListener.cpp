#include "DirtySocks/TcpListener.hpp"

#include "DirtySocks/ErrorConditions.hpp"
#include "DirtySocks/SocketAddress.hpp"
#include "DirtySocks/System.hpp"
#include "DirtySocks/TcpSocket.hpp"

namespace ds
{

void TcpListener::listen(const SocketAddress& addr, int backlog, std::error_code& ec)
{
    ec.clear();
    init_handle(addr.get_ip_version(), Socket::Protocol::TCP, ec);
    if (ec)
        return;

    if (SOCKET_ERROR == ::bind(get_handle(), &addr.get_sockaddr(), addr.get_sockaddr_len()))
    {
        ec = System::get_last_error_code();
        return;
    }

    if (SOCKET_ERROR == ::listen(get_handle(), backlog))
    {
        ec = System::get_last_error_code();
        return;
    }
}

void TcpListener::listen(const SocketAddress& addr, std::error_code& ec)
{
    return listen(addr, SOMAXCONN, ec);
}

void TcpListener::accept(TcpSocket& out_socket, SocketAddress& addr, std::error_code& ec)
{
    [[maybe_unused]] socklen_t addr_len = addr.get_sockaddr_len();
    return accept(out_socket, const_cast<sockaddr*>(&addr.get_sockaddr()), &addr_len, ec);
}

void TcpListener::accept(TcpSocket& out_socket, std::error_code& ec)
{
    return accept(out_socket, nullptr, nullptr, ec);
}

void TcpListener::accept(TcpSocket& out_socket, sockaddr* addr, socklen_t* addr_len, std::error_code& ec)
{
    ec.clear();

    SOCKET handle = ::accept(get_handle(), addr, addr_len);

    if (INVALID_SOCKET == handle)
    {
        ec = System::get_last_error_code();
        return;
    }

    out_socket = TcpSocket(handle, is_non_blocking());

    // inherit non-blocking option manually
    // (on some platforms, client socket doesn't inherit non-blocking option from listener socket)
    out_socket.set_non_blocking(is_non_blocking(), ec);
}

} // namespace ds
