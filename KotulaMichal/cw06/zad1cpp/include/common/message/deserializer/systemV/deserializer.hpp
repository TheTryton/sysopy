#pragma once

#include <common/message/deserializer/basic/deserializer.hpp>
#include <common/message/systemV/messages.hpp>

namespace systemV
{
    class message_deserializer : public imessage_deserializer
    {
    private:
        ::message_deserializer _deserializer_basic;
    public:
        virtual ~message_deserializer() = default;
    public:
        virtual unique_ptr<imessage> deserialize(message_type msg_type, byte_buffer_iterator offset) const override ;
    };
}