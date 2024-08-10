#include "TcpSocket.hpp"

namespace ds
{

TcpSocket::TcpSocket(SOCKET handle, bool blocking) : Socket(handle, blocking)
{
}

} // namespace ds
