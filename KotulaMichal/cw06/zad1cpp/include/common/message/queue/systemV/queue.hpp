#pragma once

#include <common/message/queue/queue.hpp>
#include <common/message/message.hpp>
#include <common/message/deserializer/deserializer.hpp>

namespace systemV
{
    class message_queue : public imessage_queue
    {
    private:
        struct message_header
        {
            long mtype;
            message_type real_type;
            size_t message_size;
        };

        static long translate_priority(message_priority priority);

        static message_priority translate_priority(long priority);

        static byte_buffer serialize_message_header(const imessage& msg);

        static byte_buffer serialize_message_data(const imessage& msg);
    private:
        key_t _queue_key = -1;
        int _queue_id = -1;
        bool _owner = false;
    public:
        message_queue(const string& key_path, char project_id);
        message_queue(key_t key);
    public:
        virtual ~message_queue() = default;
    public:
        key_t key() const;
    public:
        virtual unique_ptr<imessage> peek_message(imessage_deserializer& deserializer) override;
        virtual unique_ptr<imessage> receive_message(imessage_deserializer& deserializer) override;
        virtual void send_message(const imessage& msg) override;
    };
}