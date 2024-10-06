#include "TcpSocket.hpp"

#include "SocketAddress.hpp"
#include "System.hpp"

namespace ds
{

void TcpSocket::connect(const SocketAddress& addr, std::error_code& ec)
{
    ec.clear();
    init_handle(addr.get_ip_version(), Socket::Protocol::TCP, ec);
    if (ec)
        return;

    if (SOCKET_ERROR == ::connect(get_handle(), &addr.get_sockaddr(), addr.get_sockaddr_len()))
    {
        ec = System::get_last_error_code();
        return;
    }
}

void TcpSocket::send(const void* data, std::size_t data_length, std::size_t& sent_length, std::error_code& ec)
{
    ec.clear();

#ifdef _WIN32
    const auto ret = ::send(get_handle(), static_cast<const char*>(data), static_cast<int>(data_length), 0);
#else // POSIX
    const auto ret = ::send(get_handle(), static_cast<const char*>(data), data_length, 0);
#endif

    if (SOCKET_ERROR == ret)
    {
        sent_length = 0;
        ec = System::get_last_error_code();
        return;
    }

    sent_length = ret;
}

void TcpSocket::send(const void* data, std::size_t data_length, std::error_code& ec)
{
    [[maybe_unused]] std::size_t sent_length;
    return send(data, data_length, sent_length, ec);
}

void TcpSocket::receive(void* data, std::size_t data_length, std::size_t& received_length, std::error_code& ec)
{
    ec.clear();

#ifdef _WIN32
    const auto ret = ::recv(get_handle(), static_cast<char*>(data), static_cast<int>(data_length), 0);
#else // POSIX
    const auto ret = ::recv(get_handle(), static_cast<char*>(data), data_length, 0);
#endif

    if (SOCKET_ERROR == ret)
    {
        received_length = 0;
        ec = System::get_last_error_code();
        return;
    }

    received_length = ret;
}

auto TcpSocket::get_remote_address(std::error_code& ec) const -> std::optional<SocketAddress>
{
    sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);

    if (SOCKET_ERROR == ::getpeername(get_handle(), reinterpret_cast<sockaddr*>(&addr), &addr_len))
    {
        ec = System::get_last_error_code();
        return std::nullopt;
    }

    return SocketAddress(reinterpret_cast<sockaddr&>(addr));
}

TcpSocket::TcpSocket(SOCKET handle, bool non_blocking) : Socket(handle, non_blocking)
{
}

} // namespace ds
