#pragma once

#include <common/message/message.hpp>

template<message_type MT>
class message{
};

template<>
class message<message_type::LIST_REQUEST> : public client_message
{
public:
    message(client_id_t sender_id) : client_message(sender_id) {}
    message(const message& other) = default;
    message(message&& other) = default;
public:
    virtual ~message() override = default;
public:
    virtual message_type type() const override {return message_type::LIST_REQUEST;}
    virtual message_priority priority() const override {return message_priority ::NORMAL; }
};

template<>
class message<message_type::LIST_REPLY> : public imessage
{
private:
    vector<pair<client_id_t, optional<client_id_t>>> _listed_clients;
public:
    message(const vector<pair<client_id_t, optional<client_id_t>>>& listed_clients) : _listed_clients(listed_clients) {}
    message(const message& other) = default;
    message(message&& other) = default;
public:
    virtual ~message() override = default;
public:
    vector<pair<client_id_t, optional<client_id_t>>>& listed_clients() {return _listed_clients;}
    const vector<pair<client_id_t, optional<client_id_t>>>& listed_clients() const {return _listed_clients;}
public:
    virtual size_t calculate_serialized_size() const override
    {
        return serialization<decltype(_listed_clients)>::calculate_serialized_size(_listed_clients);
    }
    virtual byte_buffer_iterator serialize(byte_buffer_iterator offset) const override
    {
        return serialization<decltype(_listed_clients)>::serialize(_listed_clients, offset);
    }
    virtual message_type type() const override {return message_type::LIST_REPLY;}
    virtual message_priority priority() const override {return message_priority::NORMAL; }
};

template<>
class message<message_type::CONNECT_REQUEST> : public client_message
{
private:
    client_id_t _target_id;
public:
    message(client_id_t sender_id, client_id_t target_id) : client_message(sender_id), _target_id(target_id) {}
    message(const message& other) = default;
    message(message&& other) = default;
public:
    virtual ~message() override = default;
public:
    client_id_t target_id() const {return _target_id;};
public:
    virtual size_t calculate_serialized_size() const override
    {
        return
                serialization<decltype(_target_id)>::calculate_serialized_size(_target_id) +
                client_message::calculate_serialized_size();
    }
    virtual byte_buffer_iterator serialize(byte_buffer_iterator offset) const override
    {
        offset = client_message::serialize(offset);
        return serialization<decltype(_target_id)>::serialize(_target_id, offset);
    }
    virtual message_type type() const override {return message_type::CONNECT_REQUEST; }
    virtual message_priority priority() const override {return message_priority::NORMAL; }
};

template<>
class message<message_type::DISCONNECT> : public client_message
{
public:
    message(client_id_t sender_id) : client_message(sender_id) {}
    message(const message& other) = default;
    message(message&& other) = default;
public:
    virtual ~message() override = default;
public:
    virtual message_type type() const override {return message_type::DISCONNECT; }
    virtual message_priority priority() const override {return message_priority::NORMAL; }
};

template<>
class message<message_type::STOP> : public client_message
{
public:
    message(client_id_t sender_id) : client_message(sender_id) {}
    message(const message& other) = default;
    message(message&& other) = default;
public:
    virtual ~message() override = default;
public:
    virtual message_type type() const override {return message_type::STOP; }
    virtual message_priority priority() const override {return message_priority::HIGH; }
};

template<>
class message<message_type::SERVER_STOP> : public imessage
{
public:
    message() {}
    message(const message& other) = default;
    message(message&& other) = default;
public:
    virtual ~message() override = default;
public:
    virtual size_t calculate_serialized_size() const override
    {
        return 0;
    }
    virtual byte_buffer_iterator serialize(byte_buffer_iterator offset) const override
    {
        return offset;
    }
    virtual message_type type() const override {return message_type::SERVER_STOP;}
    virtual message_priority priority() const override {return message_priority::HIGH; }
};

template<>
class message<message_type::INIT_REPLY> final : public imessage
{
private:
    client_id_t _assigned_id;
public:
    message(client_id_t assigned_id) : _assigned_id(assigned_id) {}
    message(const message& other) = default;
    message(message&& other) = default;
public:
    virtual ~message() override = default;
public:
    client_id_t assigned_id() const {return _assigned_id;}
public:
    virtual size_t calculate_serialized_size() const override
    {
        return serialization<decltype(_assigned_id)>::calculate_serialized_size(_assigned_id);
    }
    virtual byte_buffer_iterator serialize(byte_buffer_iterator offset) const override
    {
        return serialization<decltype(_assigned_id)>::serialize(_assigned_id, offset);
    }
    virtual message_type type() const override {return message_type::INIT_REPLY; }
    virtual message_priority priority() const override {return message_priority::HIGH; }
};

template<>
class message<message_type::CHAT> : public client_message
{
private:
    string _text;
public:
    message(client_id_t sender_id, const string& text) : client_message(sender_id), _text(text) {}
    message(const message& other) = default;
    message(message&& other) = default;
public:
    virtual ~message() override = default;
public:
    string_view text() const {return _text;}
public:
    virtual size_t calculate_serialized_size() const override
    {
        return
                serialization<decltype(_text)>::calculate_serialized_size(_text) +
                client_message::calculate_serialized_size();
    }
    virtual byte_buffer_iterator serialize(byte_buffer_iterator offset) const override
    {
        offset = client_message::serialize(offset);
        return serialization<decltype(_text)>::serialize(_text, offset);
    }
    virtual message_type type() const override {return message_type::STOP; }
    virtual message_priority priority() const override {return message_priority::HIGH; }
};