#pragma once

#include <common/message/deserializer/deserializer.hpp>
#include <common/message/basic/messages.hpp>

class message_deserializer : public imessage_deserializer
{
public:
    virtual ~message_deserializer() = default;
public:
    virtual unique_ptr<imessage> deserialize(message_type msg_type, byte_buffer_iterator offset) const override;
};