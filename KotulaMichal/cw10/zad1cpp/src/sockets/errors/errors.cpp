#include <sockets/errors/errors.hpp>

SOCKETS_NAMESPACE_BEGIN

const char *socket_error_category::name() const noexcept {
    return "socket error";
}

string socket_error_category::message(int ev) const {
    return strerror(ev);
}

const char *listen_error_category::name() const noexcept {
    return "listen error";
}

string listen_error_category::message(int ev) const {
    return strerror(ev);
}

const char *accept_error_category::name() const noexcept {
    return "accept error";
}

string accept_error_category::message(int ev) const {
    return strerror(ev);
}

const char *bind_error_category::name() const noexcept {
    return "bind error";
}

string bind_error_category::message(int ev) const {
    return strerror(ev);
}

const char *connect_error_category::name() const noexcept {
    return "connect error";
}

string connect_error_category::message(int ev) const {
    return strerror(ev);
}

error_code make_error_code(socket_error ec) {
    return {static_cast<int>(ec), _socket_error_category};
}

error_code make_error_code(listen_error ec) {
    return {static_cast<int>(ec), _listen_error_category};
}

error_code make_error_code(accept_error ec) {
    return {static_cast<int>(ec), _connect_error_category};
}

error_code make_error_code(bind_error ec) {
    return {static_cast<int>(ec), _bind_error_category};
}

error_code make_error_code(connect_error ec) {
    return {static_cast<int>(ec), _connect_error_category};
}

SOCKETS_NAMESPACE_END