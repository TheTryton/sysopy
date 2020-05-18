#pragma once

#include <sockets/includes.hpp>

SOCKETS_NAMESPACE_BEGIN

using std::error_code;
using std::vector;
using std::byte;
using std::queue;
using std::function;
using std::condition_variable;
using std::mutex;
using std::scoped_lock;
using std::unique_lock;
using std::atomic;
using std::string;
using std::string_view;
using std::variant;
using std::shared_ptr;

enum class message_type
{
    login,
    authorization_result
};

struct msg_header
{
    size_t msg_size;
    message_type msg_type;
};

struct login_message
{
    msg_header header() {return {client_name.size(), message_type::login};}
    string client_name;
};

struct authorization_result
{
    msg_header header() {return {sizeof(authorization_result), message_type::authorization_result};}
    bool failed;
    int assigned_id;
};

SOCKETS_NAMESPACE_END