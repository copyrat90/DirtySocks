#pragma once

#include "Socket.hpp"

namespace ds
{

class TcpSocket final : public Socket
{
public:
    TcpSocket() = default;

private:
    friend class TcpListener;

    TcpSocket(SOCKET, bool blocking);

private:
};

} // namespace ds
