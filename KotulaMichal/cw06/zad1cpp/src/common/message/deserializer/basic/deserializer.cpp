#include <common/message/deserializer/basic/deserializer.hpp>
#include <common/message/deserializer/systemV/deserializer.hpp>

unique_ptr<imessage> message_deserializer::deserialize(message_type msg_type, byte_buffer_iterator offset) const {
    switch (msg_type) {
        case message_type::LIST_REQUEST:
            return unique_ptr<imessage>(
                    new message<message_type::LIST_REQUEST>(serialization<client_id_t>::deserialize(offset)));
        case message_type::LIST_REPLY:
            return unique_ptr<imessage>(
                    new message<message_type::LIST_REPLY>(serialization<vector<pair<client_id_t, optional<client_id_t>>>>::deserialize(offset)));
        case message_type::CONNECT_REQUEST: {
            client_id_t sender = serialization<client_id_t>::deserialize(offset);
            client_id_t target = serialization<client_id_t>::deserialize(offset);
            return unique_ptr<imessage>(new message<message_type::CONNECT_REQUEST>(sender, target));
        }
        case message_type::DISCONNECT:
            return unique_ptr<imessage>(
                    new message<message_type::DISCONNECT>(serialization<client_id_t>::deserialize(offset)));
        case message_type::STOP:
            return unique_ptr<imessage>(
                    new message<message_type::STOP>(serialization<client_id_t>::deserialize(offset)));
        case message_type::SERVER_STOP:
            return unique_ptr<imessage>(
                    new message<message_type::SERVER_STOP>());
        case message_type::INIT_REPLY:
            return unique_ptr<imessage>(
                    new message<message_type::INIT_REPLY>(serialization<client_id_t>::deserialize(offset)));
        case message_type::CHAT:
        {
            client_id_t sender = serialization<client_id_t>::deserialize(offset);
            string text = serialization<string>::deserialize(offset);
            return unique_ptr<imessage>(new message<message_type::CHAT>(sender, text));
        }
        default:
            return nullptr;
    }
}
