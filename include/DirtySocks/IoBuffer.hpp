#pragma once

#include "DirtySocks/PlatformSocket.hpp"

namespace ds
{

#ifdef _WIN32

using IoBuffer = WSABUF;
#define iov_base buf // POSIX name for buffer pointer
#define iov_len len  // POSIX name for buffer length

#else // POSIX

using IoBuffer = iovec;

#endif

} // namespace ds
