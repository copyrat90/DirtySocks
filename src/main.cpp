#include <cassert>
#include <iostream>

#include "SocketAddress.hpp"
#include "System.hpp"

int main()
{
#if defined(_WIN32) && defined(_UNICODE)
    ds::System::init(&std::wcerr);
    auto& out = std::wcout;
#else
    ds::System::init(&std::cerr);
    auto& out = std::cout;
#endif

    ds::SocketAddress addr_fixed(192, 168, 0, 1, 7777);
    out << addr_fixed.get_presentation() << '\n';

    auto addr = ds::SocketAddress::resolve(TEXT("192.168.0.1"), TEXT("30000"), ds::IpVersion::V4);
    assert(addr);
    out << addr->get_presentation() << "\n\n";

    addr = ds::SocketAddress::resolve(TEXT("example.com"), TEXT("0"), ds::IpVersion::V4);
    if (addr)
        out << addr->get_presentation() << '\n';
    else
        out << "resolve failed for IPv4 `example.com`" << '\n';

    addr = ds::SocketAddress::resolve(TEXT("example.com"), TEXT("0"), ds::IpVersion::V6);
    if (addr)
        out << addr->get_presentation() << "\n\n";
    else
        out << "resolve failed for IPv6 `example.com`" << "\n\n";

    addr = ds::SocketAddress::resolve(TEXT("localhost"), TEXT("0"), ds::IpVersion::V4);
    assert(addr);
    out << addr->get_presentation() << '\n';

    addr = ds::SocketAddress::resolve(TEXT("localhost"), TEXT("0"), ds::IpVersion::V6);
    assert(addr);
    out << addr->get_presentation() << '\n';

    ds::System::destroy();
}
