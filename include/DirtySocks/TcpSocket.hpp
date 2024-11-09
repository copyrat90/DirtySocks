#pragma once

#include "DirtySocks/IoBuffer.hpp"
#include "DirtySocks/Socket.hpp"
#include "DirtySocks/SocketAddress.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
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

    void send(std::span<IoBuffer> buffers, std::size_t& sent_length, std::error_code&);
    void send(std::span<IoBuffer> buffers, std::error_code&);

#ifdef _WIN32
    void send(std::span<IoBuffer> buffers, WSAOVERLAPPED& overlapped,
              LPWSAOVERLAPPED_COMPLETION_ROUTINE completion_routine, std::error_code&);
    void send(std::span<IoBuffer> buffers, WSAOVERLAPPED& overlapped, std::error_code&);
#endif

    void receive(void* data, std::size_t data_length, std::size_t& received_length, std::error_code&);

    void receive(std::span<IoBuffer> buffers, std::size_t& received_length, std::error_code&);

#ifdef _WIN32
    void receive(std::span<IoBuffer> buffers, WSAOVERLAPPED& overlapped,
                 LPWSAOVERLAPPED_COMPLETION_ROUTINE completion_routine, std::error_code&);
    void receive(std::span<IoBuffer> buffers, WSAOVERLAPPED& overlapped, std::error_code&);
#endif

public:
    auto get_remote_address(std::error_code&) const -> std::optional<SocketAddress>;

private:
    friend class TcpListener;

    TcpSocket(SOCKET, bool non_blocking);
};

} // namespace ds
