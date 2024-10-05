#include "TcpSocket.hpp"

#include "SocketAddress.hpp"
#include "System.hpp"

namespace ds
{

auto TcpSocket::connect(const SocketAddress& addr) -> Result
{
    init_handle(addr.get_ip_version(), Socket::Protocol::TCP);

    if (SOCKET_ERROR == ::connect(get_handle(), &addr.get_sockaddr(), addr.get_sockaddr_len()))
    {
#ifdef _WIN32
        if (WSAEWOULDBLOCK == WSAGetLastError())
#else // POSIX
        if (EINPROGRESS == errno || EAGAIN == errno)
#endif
            return Result::IN_PROGRESS;
        else
        {
            const auto result = get_result_from_error();
            if (Result::ERROR == result)
                DS_PRINT_ERROR("TcpSocket::connect() failed");

            return result;
        }
    }

    return Result::DONE;
}

auto TcpSocket::send(const void* data, std::size_t data_length, std::size_t& sent_length) -> Result
{
#ifdef _WIN32
    const auto ret = ::send(get_handle(), static_cast<const char*>(data), static_cast<int>(data_length), 0);
#else // POSIX
    const auto ret = ::send(get_handle(), static_cast<const char*>(data), data_length, 0);
#endif

    if (SOCKET_ERROR == ret)
    {
        const auto result = get_result_from_error();
        if (Result::ERROR == result)
            DS_PRINT_ERROR("TcpSocket::send() failed");

        return result;
    }

    sent_length = ret;

    if (sent_length != data_length)
        return Result::IN_PROGRESS;

    return Result::DONE;
}

auto TcpSocket::send(const void* data, std::size_t data_length) -> Result
{
    [[maybe_unused]] std::size_t sent_length;
    return send(data, data_length, sent_length);
}

auto TcpSocket::receive(void* data, std::size_t data_length, std::size_t& received_length) -> Result
{
#ifdef _WIN32
    const auto ret = ::recv(get_handle(), static_cast<char*>(data), static_cast<int>(data_length), 0);
#else // POSIX
    const auto ret = ::recv(get_handle(), static_cast<char*>(data), data_length, 0);
#endif

    if (SOCKET_ERROR == ret)
    {
        const auto result = get_result_from_error();
        if (Result::ERROR == result)
            DS_PRINT_ERROR("TcpSocket::send() failed");

        return result;
    }

    received_length = ret;

    return Result::DONE;
}

TcpSocket::TcpSocket(SOCKET handle, bool non_blocking) : Socket(handle, non_blocking)
{
}

} // namespace ds
