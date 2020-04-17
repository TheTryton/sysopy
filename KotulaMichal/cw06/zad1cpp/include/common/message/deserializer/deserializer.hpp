#pragma once

#include <common/common.hpp>
#include <common/message/message.hpp>

class imessage_deserializer
{
public:
    virtual ~imessage_deserializer() = default;
public:
    virtual unique_ptr<imessage> deserialize(message_type msg_type, byte_buffer_iterator offset) const = 0;
};