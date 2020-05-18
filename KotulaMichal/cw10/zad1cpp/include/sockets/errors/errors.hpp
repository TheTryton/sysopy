#pragma once

#include <sockets/common.hpp>

SOCKETS_NAMESPACE_BEGIN

enum class socket_error
{
    permission_denied = EACCES,
    invalid_address_family = EAFNOSUPPORT,
    unknown_protocol = EINVAL,
    program_sockets_limit_reached = EMFILE,
    system_sockets_limit_reached = ENFILE,
    not_enough_free_memory = ENOMEM,
    protocol_not_supported = EPROTONOSUPPORT
};

enum class listen_error
{
    address_in_use = EADDRINUSE,
    invalid_socket = EBADF,
    invalid_socket_id = ENOTSOCK,
    socket_does_not_support_listen = EOPNOTSUPP
};

enum class accept_error {
    no_available_client = EAGAIN,
    invalid_socket = EBADF,
    connection_aborted = ECONNABORTED,
    non_mutable_address = EFAULT,
    interrupted = EINTR,
    socket_not_listening = EINVAL,
    program_sockets_limit_reached = EMFILE,
    system_sockets_limit_reached = ENFILE,
    not_enough_free_memory = ENOMEM,
    invalid_socket_id = ENOTSOCK,
    socket_is_not_tcp = EOPNOTSUPP,
    protocol_error = EPROTO,
    firewall_error = EPERM
};

enum class bind_error
{
    invalid_address_pointer_or_permission = EACCES,
    address_in_use = EADDRINUSE,
    invalid_socket = EBADF,
    already_bound_or_invalid_address = EINVAL,
    invalid_socket_id = ENOTSOCK,
    address_not_available = EADDRNOTAVAIL,
    non_mutable_address = EFAULT,
    too_many_links = ELOOP,
    too_long_address = ENAMETOOLONG,
    socket_path_invalid = ENOENT,
    not_enough_free_memory = ENOMEM,
    read_only_socket = EROFS
};

enum class connect_error {
    access_error = EACCES,
    socket_not_broadcast = EPERM,
    address_in_use = EADDRINUSE,
    address_not_available = EADDRNOTAVAIL,
    invalid_address_family = EAFNOSUPPORT,
    retry = EAGAIN,
    connection_not_completed = EALREADY,
    invalid_socket = EBADF,
    connection_refused = ECONNREFUSED,
    non_mutable_address = EFAULT,
    connection_in_progress = EINPROGRESS,
    interrupted = EINTR,
    already_connected = EISCONN,
    network_unreachable = ENETUNREACH,
    invalid_socket_id = ENOTSOCK,
    protocol_not_supported = EPROTOTYPE,
    timed_out = ETIMEDOUT
};

struct socket_error_category : std::error_category {
    const char *name() const noexcept override;
    string message(int ev) const override;
};

struct listen_error_category : std::error_category {
    const char *name() const noexcept override;
    string message(int ev) const override;
};

struct accept_error_category : std::error_category {
    const char *name() const noexcept override;
    string message(int ev) const override;
};

struct bind_error_category : std::error_category {
    const char *name() const noexcept override;
    string message(int ev) const override;
};

struct connect_error_category : std::error_category {
    const char *name() const noexcept override;
    string message(int ev) const override;
};

const socket_error_category _socket_error_category{};
const listen_error_category _listen_error_category{};
const accept_error_category _accept_error_category{};
const bind_error_category _bind_error_category{};
const connect_error_category _connect_error_category{};

error_code make_error_code(socket_error ec);
error_code make_error_code(listen_error ec);
error_code make_error_code(accept_error ec);
error_code make_error_code(bind_error ec);
error_code make_error_code(connect_error ec);

SOCKETS_NAMESPACE_END

namespace std
{
template<>
struct is_error_code_enum<SOCKETS_NAMESPACE_FULL::listen_error> : true_type {};
template<>
struct is_error_code_enum<SOCKETS_NAMESPACE_FULL::accept_error> : true_type {};
template<>
struct is_error_code_enum<SOCKETS_NAMESPACE_FULL::bind_error> : true_type {};
template<>
struct is_error_code_enum<SOCKETS_NAMESPACE_FULL::connect_error> : true_type {};
}