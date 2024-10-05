#pragma once

#include "Socket.hpp"

#include <cstddef>
#include <cstdint>

namespace ds
{

class TcpSocket final : public Socket
{
public:
    TcpSocket() = default;

public:
    auto connect(const SocketAddress&) -> Result;

    auto send(const void* data, std::size_t data_length, std::size_t& sent_length) -> Result;
    auto send(const void* data, std::size_t data_length) -> Result;

    auto receive(void* data, std::size_t data_length, std::size_t& received_length) -> Result;

private:
    friend class TcpListener;

    TcpSocket(SOCKET, bool non_blocking);
};

} // namespace ds
