#pragma once

#include "DirtySocks/Socket.hpp"

#include <cstdint>
#include <system_error>

namespace ds
{

class SocketAddress;
class TcpSocket;

class TcpListener final : public Socket
{
public:
    void listen(const SocketAddress&, std::error_code&);
    void accept(TcpSocket& out_socket, std::error_code&);
};

} // namespace ds
