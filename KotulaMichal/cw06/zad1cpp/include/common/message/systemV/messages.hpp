#pragma once

#include <common/message/message.hpp>

namespace systemV
{
    template<message_type MT>
    class message{
    };

    template<>
    class message<message_type::CONNECT_REPLY> : public client_message
    {
    private:
        optional<key_t> _target_key;
    public:
        message(client_id_t sender_id, optional<key_t> target_key) : client_message(sender_id), _target_key(target_key) {}
        message(const message& other) = default;
        message(message&& other) = default;
    public:
        virtual ~message() override = default;
    public:
        optional<key_t> target_key() const {return _target_key;}
    public:
        virtual size_t calculate_serialized_size() const override
        {
            return
                    serialization<decltype(_target_key)>::calculate_serialized_size(_target_key) +
                    client_message::calculate_serialized_size();
        }
        virtual byte_buffer_iterator serialize(byte_buffer_iterator offset) const override
        {
            offset = client_message::serialize(offset);
            return serialization<decltype(_target_key)>::serialize(_target_key, offset);
        }
        virtual message_type type() const override {return message_type::CONNECT_REPLY; }
        virtual message_priority priority() const override {return message_priority::NORMAL; }
    };

    template<>
    class message<message_type::INIT> final : public imessage
    {
    private:
        key_t _client_queue_key;
    public:
        message(key_t client_queue_key) : _client_queue_key(client_queue_key) {}
        message(const message& other) = default;
        message(message&& other) = default;
    public:
        virtual ~message() override = default;
    public:
        key_t client_queue_key() const {return _client_queue_key;}
    public:
        virtual size_t calculate_serialized_size() const override
        {
            return serialization<decltype(_client_queue_key)>::calculate_serialized_size(_client_queue_key);
        }
        virtual byte_buffer_iterator serialize(byte_buffer_iterator offset) const override
        {
            return serialization<decltype(_client_queue_key)>::serialize(_client_queue_key, offset);
        }
        virtual message_type type() const override {return message_type::INIT; }
        virtual message_priority priority() const override {return message_priority::HIGH; }
    };
}