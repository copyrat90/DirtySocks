#include "System.hpp"

#include "platform_socket.hpp"

#include "ErrorCodes.hpp"

namespace ds
{

void System::init(std::error_code& ec)
{
    ec.clear();

#ifdef _WIN32
    WSADATA wsa_data;

    ec = static_cast<SystemErrc>(WSAStartup(MAKEWORD(2, 2), &wsa_data));

    if (ec)
        return;

    if (LOBYTE(wsa_data.wVersion) != 2 || HIBYTE(wsa_data.wVersion) != 2)
    {
        ec = SystemErrc::version_not_supported;
        return;
    }

#endif
}

void System::destroy()
{
#ifdef _WIN32
    WSACleanup();
#endif
}

auto System::get_last_error_code() -> std::error_code
{
#ifdef _WIN32
    return static_cast<SystemErrc>(WSAGetLastError());
#else // POSIX
    return static_cast<SystemErrc>(errno);
#endif
}

} // namespace ds
