#pragma once

#include <common/common.hpp>

class imessage_queue
{
public:
    virtual ~imessage_queue() = default;
public:
    virtual unique_ptr<imessage> peek_message(imessage_deserializer& deserializer) = 0;
    virtual unique_ptr<imessage> receive_message(imessage_deserializer& deserializer) = 0;
    virtual void send_message(const imessage& msg) = 0;
};