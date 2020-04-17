#pragma once

#include <common/common.hpp>
#include <common/serialization/serialization.hpp>

enum class message_type
{
    STOP,
    SERVER_STOP,
    CONNECT_REQUEST,
    CONNECT_REPLY,
    DISCONNECT,
    LIST_REQUEST,
    LIST_REPLY,
    INIT,
    INIT_REPLY,
    CHAT
};

enum class message_priority {
    HIGH,
    NORMAL,
    LOW
};

class imessage
{
public:
    virtual ~imessage() = default;
public:
    virtual size_t calculate_serialized_size() const = 0;
    virtual byte_buffer_iterator serialize(byte_buffer_iterator offset) const = 0;
    virtual message_type type() const = 0;
    virtual message_priority priority() const = 0;
};

class client_message : public imessage
{
private:
    client_id_t _sender_id;
public:
    client_message(client_id_t sender_id);
    client_message(const client_message& other) = default;
    client_message(client_message&& other) = default;
public:
    virtual ~client_message() override = default;
public:
    client_id_t sender_id() const;
public:
    virtual size_t calculate_serialized_size() const override;
    virtual byte_buffer_iterator serialize(byte_buffer_iterator offset) const override;
};