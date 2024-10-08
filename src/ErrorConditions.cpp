#include "DirtySocks/ErrorConditions.hpp"

#include "DirtySocks/ErrorCodes.hpp"

namespace ds
{

namespace
{

class SocketErrorCategory : public std::error_category
{
public:
    static auto instance() -> SocketErrorCategory&
    {
        static SocketErrorCategory category;
        return category;
    }

public:
    auto name() const noexcept -> const char* override
    {
        return "DirtySocks::SocketError";
    }

    auto message(int error_value) const -> std::string override
    {
        switch (static_cast<SocketErrc>(error_value))
        {
        case SocketErrc::OK:
            return "OK";
        case SocketErrc::WOULD_BLOCK:
            return "Would block";
        case SocketErrc::DISCONNECTED:
            return "Disconnected";

        default:
            break;
        }
        return "Unknown SocketError condition";
    }

public:
    bool equivalent(const std::error_code& ec, int cond) const noexcept override
    {
        switch (static_cast<SocketErrc>(cond))
        {
        case SocketErrc::OK:
            return !ec;

        case SocketErrc::WOULD_BLOCK:
            return ec == SystemErrc::operation_would_block || ec == SystemErrc::resource_unavailable_try_again;

        case SocketErrc::DISCONNECTED:
            return ec == SystemErrc::network_reset || ec == SystemErrc::connection_aborted ||
                   ec == SystemErrc::connection_reset || ec == SystemErrc::broken_pipe || ec == SystemErrc::timed_out;

        default:
            break;
        }

        return false;
    }

private:
    SocketErrorCategory() = default;
};

} // namespace

auto make_error_condition(SocketErrc errc) -> std::error_condition
{
    return std::error_condition(static_cast<int>(errc), SocketErrorCategory::instance());
}

} // namespace ds
