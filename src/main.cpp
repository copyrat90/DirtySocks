#include <cassert>
#include <iostream>

#include "IpAddress.hpp"
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

    ds::IpAddress addr1(192, 168, 0, 1, 7777);
    out << addr1.get_presentation() << '\n';

    auto addr2 = ds::IpAddress::resolve(TEXT("example.com"), TEXT("http"));
    if (addr2)
        out << addr2->get_presentation() << '\n';
    else
        out << "resolve failed!" << '\n';

    auto addr3 = ds::IpAddress::resolve(TEXT("192.168.0.1:7878"));
    assert(addr3);
    out << addr3->get_presentation() << '\n';

    ds::System::destroy();
}
