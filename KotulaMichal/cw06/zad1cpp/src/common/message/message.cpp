#include <common/message/message.hpp>

client_message::client_message(client_id_t sender_id)
    : _sender_id(sender_id)
{

}

client_id_t client_message::sender_id() const {
    return _sender_id;
}

size_t client_message::calculate_serialized_size() const {
    return serialization<decltype(_sender_id)>::calculate_serialized_size(_sender_id);
}

byte_buffer_iterator client_message::serialize(byte_buffer_iterator offset) const {
    return serialization<decltype(_sender_id)>::serialize(_sender_id, offset);
}
