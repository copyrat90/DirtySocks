#pragma once

#include "Socket.hpp"

#include <cstdint>

namespace ds
{

class SocketAddress;
class TcpSocket;

class TcpListener final : public Socket
{
public:
    auto listen(const SocketAddress&) -> Result;
    auto accept(TcpSocket& out_socket) -> Result;
};

} // namespace ds
