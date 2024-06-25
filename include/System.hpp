#pragma once

#include <ostream>
#include <string_view>

#include "platform_unicode.hpp"

#define DS_PRINT_ERROR(msg) ds::System::print_error(TEXT(msg))

namespace ds
{

class System final
{
private:
    static inline error_stream_t* _err = nullptr;

public:
    static bool init(error_stream_t* err = nullptr);
    static void destroy();

public:
    static void print_error(string_view_t error_message);

private:
    static int get_error_code();
};

} // namespace ds
