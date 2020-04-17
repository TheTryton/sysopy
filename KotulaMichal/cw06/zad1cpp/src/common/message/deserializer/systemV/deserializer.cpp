#include <common/message/deserializer/systemV/deserializer.hpp>

unique_ptr<imessage> systemV::message_deserializer::deserialize(message_type msg_type, byte_buffer_iterator offset) const {
    if(auto message = _deserializer_basic.deserialize(msg_type, offset))
    {
        return message;
    } else{
        switch(msg_type)
        {
            case message_type::CONNECT_REPLY:
            {
                client_id_t sender = serialization<client_id_t>::deserialize(offset);
                key_t target_key = serialization<key_t>::deserialize(offset);
                return unique_ptr<imessage>(new systemV::message<message_type::CONNECT_REPLY>(sender, target_key));
            }
            case message_type::INIT:
                return unique_ptr<imessage>(new systemV::message<message_type::INIT>(serialization<key_t>::deserialize(offset)));
            default:
                return nullptr;
        }
    }
}

