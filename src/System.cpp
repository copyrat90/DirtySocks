#include "System.hpp"

#include <cstring>
#include <format>

#include "platform_socket.hpp"
#include "platform_unicode.hpp"

namespace ds
{

bool System::init(error_stream_t* err)
{
    _err = err;

#ifdef _WIN32
    WSADATA wsa_data;

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
    {
        print_error(TEXT("WSAStartup() failed"));
        return false;
    }

    if (LOBYTE(wsa_data.wVersion) != 2 || HIBYTE(wsa_data.wVersion) != 2)
    {
        print_error(TEXT("Winsock2 version 2.2 not available"));
        return false;
    }

#endif
    return true;
}

void System::destroy()
{
#ifdef _WIN32
    WSACleanup();
#endif

    _err = nullptr;
}

void System::print_error(string_view_t error_message)
{
    if (!_err)
        return;

#ifdef _WIN32
    (*_err) << std::format(TEXT("[{}] {}\n"), get_error_code(), error_message);
#else // POSIX
    (*_err) << std::format("[{}] {}\n", get_error_code(), error_message);
#endif
}

int System::get_error_code()
{
#ifdef _WIN32
    return WSAGetLastError();
#else // POSIX
    return errno;
#endif
}

} // namespace ds
