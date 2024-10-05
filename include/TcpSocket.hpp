#pragma once

#include "Socket.hpp"

#include <cstddef>
#include <cstdint>
#include <system_error>

namespace ds
{

class TcpSocket final : public Socket
{
public:
    TcpSocket() = default;

public:
    void connect(const SocketAddress&, std::error_code&);

    void send(const void* data, std::size_t data_length, std::size_t& sent_length, std::error_code&);
    void send(const void* data, std::size_t data_length, std::error_code&);

    void receive(void* data, std::size_t data_length, std::size_t& received_length, std::error_code&);

private:
    friend class TcpListener;

    TcpSocket(SOCKET, bool non_blocking);
};

} // namespace ds
