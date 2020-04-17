#include <common/message/queue/systemV/queue.hpp>

long systemV::message_queue::translate_priority(message_priority priority)
{
    switch(priority)
    {
        case message_priority::HIGH:
            return 1;
        case message_priority::NORMAL:
            return 2;
        case message_priority::LOW:
        default:
            return 3;
    }
    return 4;
}

message_priority systemV::message_queue::translate_priority(long priority)
{
    switch(priority)
    {
        case 1:
            return message_priority::HIGH;
        case 2:
            return message_priority::NORMAL;
        case 3:
        default:
            return message_priority::LOW;
    }
}

byte_buffer systemV::message_queue::serialize_message_header(const imessage &msg)
{
    byte_buffer header_buffer(sizeof(message_header));

    message_header header {
            translate_priority(msg.priority()),
            msg.type(),
            sizeof(long) + msg.calculate_serialized_size()
    };

    std::copy(reinterpret_cast<byte*>(&header), reinterpret_cast<byte*>(&header) + sizeof(message_header), header_buffer.begin());

    return header_buffer;
}

byte_buffer systemV::message_queue::serialize_message_data(const imessage &msg)
{
    byte_buffer data_buffer(sizeof(long) + msg.calculate_serialized_size());
    auto b = data_buffer.begin();

    {
        auto priority = translate_priority(msg.priority());
        b = std::copy(reinterpret_cast<byte*>(&priority), reinterpret_cast<byte*>(&priority) + sizeof(priority), b);
    }

    if(msg.serialize(b) != data_buffer.end())
    {
        throw std::runtime_error("Failed to serialize message data, calculated buffer size is too big");
    }

    return data_buffer;
}

systemV::message_queue::message_queue(const string &key_path, char project_id)
    : _owner(true)
{
    if((_queue_key = ftok(key_path.c_str(), project_id)) == -1)
    {
        throw std::runtime_error("Failed to generate systemV message_queue key");
    }

    if((_queue_id = msgget(_queue_key, IPC_CREAT | 0660)) == -1)
    {
        throw std::runtime_error("Failed to create systemV message_queue");
    }
}

systemV::message_queue::message_queue(key_t key)
    : _queue_key(key)
{
    if ((_queue_id = msgget(_queue_key, 0)) == -1)
    {
        throw std::runtime_error("Failed to retrieve systemV message_queue");
    }
}

unique_ptr<imessage> systemV::message_queue::peek_message(imessage_deserializer &deserializer)
{
    message_header header;

    size_t received = 0;

    if((received = msgrcv(_queue_id, &header, sizeof(header), 0, IPC_NOWAIT)) == -1)
    {
        if(errno != ENOMSG)
        {
            throw std::runtime_error("Failed to receive message header from systemV message_queue");
        }
        else {
            return unique_ptr<imessage>(); // there is no message waiting on the queue
        }
    }

    while(received != sizeof(header))
    {
        if((received += msgrcv(_queue_id, reinterpret_cast<byte*>(&header) + received, sizeof(header) - received, 0, 0)) == -1)
        {
            throw std::runtime_error("Failed to receive message from systemV message_queue");
        }
    }

    byte_buffer data(header.message_size);

    if((received = msgrcv(_queue_id, data.data(), data.size(), 0, 0)) == -1)
    {
        throw std::runtime_error("Failed to receive message from systemV message_queue");
    }

    while(received != data.size())
    {
        if((received += msgrcv(_queue_id, data.data() + received, data.size() - received, 0, 0)) == -1)
        {
            throw std::runtime_error("Failed to receive message from systemV message_queue");
        }
    }

    auto b = data.begin() + sizeof(long);

    if(auto msg = deserializer.deserialize(header.real_type, b))
    {
        return msg;
    } else{
        throw std::runtime_error("Failed to deserialize message received from systemV message_queue");
    }
}

unique_ptr<imessage> systemV::message_queue::receive_message(imessage_deserializer &deserializer)
{
    message_header header;

    size_t received = 0;

    if((received = msgrcv(_queue_id, &header, sizeof(header), 0, 0)) == -1) {
        throw std::runtime_error("Failed to receive message header from systemV message_queue");
    }

    while(received != sizeof(header))
    {
        if((received += msgrcv(_queue_id, reinterpret_cast<byte*>(&header) + received, sizeof(header) - received, 0, 0)) == -1)
        {
            throw std::runtime_error("Failed to receive message from systemV message_queue");
        }
    }

    byte_buffer data(header.message_size);

    if((received = msgrcv(_queue_id, data.data(), data.size(), 0, 0)) == -1)
    {
        throw std::runtime_error("Failed to receive message from systemV message_queue");
    }

    while(received != data.size())
    {
        if((received += msgrcv(_queue_id, data.data() + received, data.size() - received, 0, 0)) == -1)
        {
            throw std::runtime_error("Failed to receive message from systemV message_queue");
        }
    }

    auto b = data.begin() + sizeof(long);

    if(auto msg = deserializer.deserialize(header.real_type, b))
    {
        return msg;
    } else{
        throw std::runtime_error("Failed to deserialize message received from systemV message_queue");
    }
}

void systemV::message_queue::send_message(const imessage &msg)
{
    auto header = serialize_message_header(msg);
    auto data = serialize_message_data(msg);

    if(msgsnd(_queue_id, header.data(), header.size(), 0) == -1)
    {
        throw std::runtime_error("Failed to send message header to systemV message_queue");
    }

    if(msgsnd(_queue_id, data.data(), data.size(), 0) == -1)
    {
        throw std::runtime_error("Failed to send message data to systemV message_queue");
    }
}

key_t systemV::message_queue::key() const {
    return _queue_key;
}
