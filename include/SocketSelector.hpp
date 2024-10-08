#pragma once

#include "platform_socket.hpp"

#include <system_error>
#include <unordered_set>

#ifndef _WIN32 // POSIX
#include <optional>
#endif

namespace ds
{

class Socket;

/// @brief BSD `::select()` wrapper.
///
/// Unlike BSD `::select()`, this retains added sockets after `select()` call.
///
/// This only stores raw pointers to added sockets, so added sockets should outlive this to avoid dangling pointers.
class SocketSelector final
{
public:
    SocketSelector();

public:
    void select(timeval* timeout, std::error_code&);

    // result is never changed until another `select()` is called
    bool has_read(const Socket&) const;
    bool has_write(const Socket&) const;
    bool has_except(const Socket&) const;

public:
    void add_to_read_set(const Socket&);
    void add_to_write_set(const Socket&);
    void add_to_except_set(const Socket&);

    void remove_from_read_set(const Socket&);
    void remove_from_write_set(const Socket&);
    void remove_from_except_set(const Socket&);

    void clear_read_set();
    void clear_write_set();
    void clear_except_set();

private:
    std::unordered_set<const Socket*> _read_sockets;
    std::unordered_set<const Socket*> _write_sockets;
    std::unordered_set<const Socket*> _except_sockets;

private:
    fd_set _read_set;
    fd_set _write_set;
    fd_set _except_set;

#ifndef _WIN32 // POSIX
private:
    auto get_max_read_fd() -> SOCKET;
    auto get_max_write_fd() -> SOCKET;
    auto get_max_except_fd() -> SOCKET;

private:
    std::optional<SOCKET> _max_read_fd;
    std::optional<SOCKET> _max_write_fd;
    std::optional<SOCKET> _max_except_fd;
#endif
};

} // namespace ds
