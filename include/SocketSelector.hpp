#pragma once

#include "platform_socket.hpp"

#include <cstddef>
#include <system_error>

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
    /// @return number of sockets contained in resulting selections
    int select(timeval* timeout, std::error_code&);

    // result is never changed until another `select()` is called
    bool has_read(const Socket&) const;
    bool has_write(const Socket&) const;
    bool has_except(const Socket&) const;

public:
    void add_to_read_set(const Socket&, std::error_code&);
    void add_to_write_set(const Socket&, std::error_code&);
    void add_to_except_set(const Socket&, std::error_code&);

    void remove_from_read_set(const Socket&);
    void remove_from_write_set(const Socket&);
    void remove_from_except_set(const Socket&);

    void clear_read_set();
    void clear_write_set();
    void clear_except_set();

private:
    struct Set
    {
    public:
        Set();

        bool has(const Socket&) const;

        void add(const Socket&, std::error_code&);
        void remove(const Socket&);
        void clear();

    public:
        std::size_t sockets_count = 0; // number of sockets

        fd_set all;    // retain added sockets here
        fd_set result; // result of `::select()`
#ifndef _WIN32
        SOCKET max_fd = INVALID_SOCKET; // used for `nfds` calculation
#endif
    };

private:
    Set _read_set;
    Set _write_set;
    Set _except_set;
};

} // namespace ds
