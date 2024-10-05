#pragma once

#include <system_error>

namespace ds
{

class System final
{
public:
    // This must be called first before using DirtySocks
    static void init(std::error_code&);

    static void destroy();

public:
    static auto get_last_error_code() -> std::error_code;
};

} // namespace ds
