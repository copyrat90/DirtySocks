#pragma once

#include "platform_socket.hpp"

#include <system_error>
#include <type_traits>

#ifdef _WIN32
#include <winerror.h>
#endif

#ifdef _WIN32
#define DS_WSA_PREFIX(e) WSA##e
#define DS_WIN_OR_POSIX(e_win, e_posix) e_win
#else // POSIX
#define DS_WSA_PREFIX(e) e
#define DS_WIN_OR_POSIX(e_win, e_posix) e_posix
#endif

#define NULL_ERROR_VAL 987654321

namespace ds
{

enum class AddrInfoErrc;
enum class SystemErrc;

auto make_error_code(AddrInfoErrc) -> std::error_code;
auto make_error_code(SystemErrc) -> std::error_code;

enum class AddrInfoErrc
{
    resource_unavailable_try_again = DS_WIN_OR_POSIX(WSATRY_AGAIN, EAI_AGAIN),
    invalid_argument = DS_WIN_OR_POSIX(WSAEINVAL, EAI_BADFLAGS),
    state_not_recoverable = DS_WIN_OR_POSIX(WSANO_RECOVERY, EAI_FAIL),
    address_family_not_supported = DS_WIN_OR_POSIX(WSAEAFNOSUPPORT, EAI_FAMILY),
    not_enough_memory = DS_WIN_OR_POSIX(WSA_NOT_ENOUGH_MEMORY, EAI_MEMORY),
    host_not_found = DS_WIN_OR_POSIX(WSAHOST_NOT_FOUND, EAI_NONAME),
    service_not_supported = DS_WIN_OR_POSIX(WSATYPE_NOT_FOUND, EAI_SERVICE),
    socktype_not_supported = DS_WIN_OR_POSIX(WSAESOCKTNOSUPPORT, EAI_SOCKTYPE),
};

enum class SystemErrc
{
    // POSIX error codes (also contained in `std::errc`)
    address_family_not_supported = DS_WSA_PREFIX(EAFNOSUPPORT),
    address_in_use = DS_WSA_PREFIX(EADDRINUSE),
    address_not_available = DS_WSA_PREFIX(EADDRNOTAVAIL),
    already_connected = DS_WSA_PREFIX(EISCONN),
    argument_list_too_long = DS_WIN_OR_POSIX(WSAEINVAL, E2BIG),
    argument_out_of_domain = DS_WIN_OR_POSIX(WSAEINVAL, EDOM),
    bad_address = DS_WSA_PREFIX(EFAULT),
    bad_file_descriptor = DS_WSA_PREFIX(EBADF),
    bad_message = DS_WIN_OR_POSIX(NULL_ERROR_VAL, EBADMSG),
    broken_pipe = DS_WIN_OR_POSIX(ERROR_BROKEN_PIPE, EPIPE),
    connection_aborted = DS_WSA_PREFIX(ECONNABORTED),
    connection_already_in_progress = DS_WSA_PREFIX(EALREADY),
    connection_refused = DS_WSA_PREFIX(ECONNREFUSED),
    connection_reset = DS_WSA_PREFIX(ECONNRESET),
    cross_device_link = DS_WIN_OR_POSIX(NULL_ERROR_VAL, EXDEV),
    destination_address_required = DS_WSA_PREFIX(EDESTADDRREQ),
    device_or_resource_busy = DS_WIN_OR_POSIX(NULL_ERROR_VAL, EBUSY),
    directory_not_empty = DS_WSA_PREFIX(ENOTEMPTY),
    executable_format_error = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ENOEXEC),
    file_exists = DS_WIN_OR_POSIX(NULL_ERROR_VAL, EEXIST),
    file_too_large = DS_WIN_OR_POSIX(NULL_ERROR_VAL, EFBIG),
    filename_too_long = DS_WSA_PREFIX(ENAMETOOLONG),
    function_not_supported = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ENOSYS),
    host_unreachable = DS_WSA_PREFIX(EHOSTUNREACH),
    identifier_removed = DS_WIN_OR_POSIX(NULL_ERROR_VAL, EIDRM),
    illegal_byte_sequence = DS_WIN_OR_POSIX(NULL_ERROR_VAL, EILSEQ),
    inappropriate_io_control_operation = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ENOTTY),
    interrupted = DS_WSA_PREFIX(EINTR),
    invalid_argument = DS_WSA_PREFIX(EINVAL),
    invalid_seek = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ESPIPE),
    io_error = DS_WIN_OR_POSIX(NULL_ERROR_VAL, EIO),
    is_a_directory = DS_WIN_OR_POSIX(NULL_ERROR_VAL, EISDIR),
    message_size = DS_WSA_PREFIX(EMSGSIZE),
    network_down = DS_WSA_PREFIX(ENETDOWN),
    network_reset = DS_WSA_PREFIX(ENETRESET),
    network_unreachable = DS_WSA_PREFIX(ENETUNREACH),
    no_buffer_space = DS_WSA_PREFIX(ENOBUFS),
    no_child_process = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ECHILD),
    no_link = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ENOLINK),
    no_lock_available = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ENOLCK),
    no_message_available = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ENODATA),
    no_message = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ENOMSG),
    no_protocol_option = DS_WSA_PREFIX(ENOPROTOOPT),
    no_space_on_device = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ENOSPC),
    no_stream_resources = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ENOSR),
    no_such_device_or_address = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ENXIO),
    no_such_device = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ENODEV),
    no_such_file_or_directory = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ENOENT),
    no_such_process = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ESRCH),
    not_a_directory = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ENOTDIR),
    not_a_socket = DS_WSA_PREFIX(ENOTSOCK),
    not_a_stream = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ENOSTR),
    not_connected = DS_WSA_PREFIX(ENOTCONN),
    not_enough_memory = DS_WIN_OR_POSIX(WSA_NOT_ENOUGH_MEMORY, ENOMEM),
    not_supported = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ENOTSUP),
    operation_canceled = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ECANCELED),
    operation_in_progress = DS_WSA_PREFIX(EINPROGRESS),
    operation_not_permitted = DS_WIN_OR_POSIX(ERROR_ACCESS_DENIED, EPERM),
    operation_not_supported = DS_WSA_PREFIX(EOPNOTSUPP),
    operation_would_block = DS_WSA_PREFIX(EWOULDBLOCK),
    owner_dead = DS_WIN_OR_POSIX(NULL_ERROR_VAL, EOWNERDEAD),
    permission_denied = DS_WIN_OR_POSIX(ERROR_ACCESS_DENIED, EACCES),
    protocol_error = DS_WIN_OR_POSIX(NULL_ERROR_VAL, EPROTO),
    protocol_not_supported = DS_WSA_PREFIX(EPROTONOSUPPORT),
    read_only_file_system = DS_WIN_OR_POSIX(NULL_ERROR_VAL, EROFS),
    resource_deadlock_would_occur = DS_WIN_OR_POSIX(ERROR_POSSIBLE_DEADLOCK, EDEADLK),
    resource_unavailable_try_again = DS_WIN_OR_POSIX(ERROR_RETRY, EAGAIN),
    result_out_of_range = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ERANGE),
    state_not_recoverable = DS_WIN_OR_POSIX(WSANO_RECOVERY, ENOTRECOVERABLE),
    stream_timeout = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ETIME),
    text_file_busy = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ETXTBSY),
    timed_out = DS_WSA_PREFIX(ETIMEDOUT),
    too_many_files_open_in_system = DS_WIN_OR_POSIX(NULL_ERROR_VAL, ENFILE),
    too_many_files_open = DS_WSA_PREFIX(EMFILE),
    too_many_links = DS_WIN_OR_POSIX(NULL_ERROR_VAL, EMLINK),
    too_many_symbolic_link_levels = DS_WSA_PREFIX(ELOOP),
    value_too_large = DS_WIN_OR_POSIX(NULL_ERROR_VAL, EOVERFLOW),
    wrong_protocol_type = DS_WSA_PREFIX(EPROTOTYPE),

    // Win32 only error codes
    version_not_supported = DS_WIN_OR_POSIX(WSAVERNOTSUPPORTED, NULL_ERROR_VAL),
    network_subsystem_unavailable = DS_WIN_OR_POSIX(WSASYSNOTREADY, NULL_ERROR_VAL),
    too_many_processes_open = DS_WIN_OR_POSIX(WSAEPROCLIM, NULL_ERROR_VAL),
};

} // namespace ds

namespace std
{

template <>
struct is_error_code_enum<ds::AddrInfoErrc> : true_type
{
};

template <>
struct is_error_code_enum<ds::SystemErrc> : true_type
{
};

} // namespace std
