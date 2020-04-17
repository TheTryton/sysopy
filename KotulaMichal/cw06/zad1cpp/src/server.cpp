#include <common/message/queue/systemV/queue.hpp>
#include <common/message/deserializer/systemV/deserializer.hpp>
#include <unistd.h>

using namespace std;

struct client_info
{
    client_id_t id;
    key_t client_queue_key;
    unique_ptr<imessage_queue> client_queue;
    optional<client_id_t> connected_client;
};

client_id_t generate_new_client_id()
{
    static client_id_t generated_id = 0;
    return generated_id++;
}

map<client_id_t, client_info> _clients;
bool stop_server = false;
unique_ptr<imessage_queue> server_queue;
unique_ptr<imessage_deserializer> message_deserializer;

void handle_init_message(const systemV::message<message_type::INIT>& msg)
{
    client_info new_client_info
    {
        generate_new_client_id(),
        msg.client_queue_key(),
        unique_ptr<imessage_queue>(new systemV::message_queue(msg.client_queue_key())),
        nullopt
    };
    auto new_client = _clients.insert({new_client_info.id, std::move(new_client_info)});
    new_client.first->second.client_queue->send_message(message<message_type::INIT_REPLY>(new_client.first->second.id));
}

void handle_list_message(const message<message_type::LIST_REQUEST>& msg)
{
    vector<pair<client_id_t, optional<client_id_t>>> clients(_clients.size());
    transform(_clients.begin(), _clients.end(), clients.begin(), [](const auto& pr){
        return make_pair(pr.second.id, pr.second.connected_client);
    });

    _clients[msg.sender_id()].client_queue->send_message(message<message_type::LIST_REPLY>(clients));
}

void handle_stop_message(const message<message_type::STOP>& msg)
{
    _clients.erase(msg.sender_id());
}

void handle_disconnect_message(const message<message_type::DISCONNECT>& msg)
{
    auto client = _clients.find(msg.sender_id());
    if(client != _clients.end())
    {
        if(client->second.connected_client)
        {
            auto connected_client = _clients.find(*client->second.connected_client);

            if(connected_client->second.connected_client)
            {
                connected_client->second.client_queue->send_message(msg);
            }

            connected_client->second.connected_client = nullopt;
            client->second.connected_client = nullopt;
        }
    }
}

void handle_connect_message(const message<message_type::CONNECT_REQUEST>& msg) {
    auto client = _clients.find(msg.sender_id());
    auto target = _clients.find(msg.target_id());

    if (client != _clients.end() && target != _clients.end()) {
        if(!client->second.connected_client && !target->second.connected_client)
        {
            target->second.client_queue->send_message(systemV::message<message_type::CONNECT_REPLY>(msg.sender_id(), client->second.client_queue_key));
            client->second.client_queue->send_message(systemV::message<message_type::CONNECT_REPLY>(msg.sender_id(), target->second.client_queue_key));

            return;
        }
    }

    // in case of fail

    if (client != _clients.end()) {
        client->second.client_queue->send_message(
                systemV::message<message_type::CONNECT_REPLY>(msg.sender_id(), nullopt));
    }
}

void handle_server_stop()
{
    message<message_type::SERVER_STOP> server_stop;

    for(auto& c : _clients)
    {
        c.second.client_queue->send_message(server_stop);
    }

    size_t disconnected_clients = 0;

    while(disconnected_clients != _clients.size())
    {
        if(server_queue->receive_message(*message_deserializer)->type() == message_type::STOP)
        {
            disconnected_clients++;
        }
    }
}

void handle_terminal_interrupt_signal(int signum) {
    stop_server = true;
};

int main(int argc, char** argv)
{
    signal(SIGINT, handle_terminal_interrupt_signal);

    auto server_key_path = "/home/michal/Desktop/operating_systems/KotulaMichal/cw06/";
    auto project_id = 'M';

    try{
        server_queue = unique_ptr<imessage_queue>(new systemV::message_queue(server_key_path, project_id));
        cout << "Server message_queue key: " << static_cast<systemV::message_queue*>(server_queue.get())->key() << endl;
        message_deserializer = unique_ptr<imessage_deserializer>(new systemV::message_deserializer());

        while(!stop_server)
        {
            if(auto message = server_queue->peek_message(*message_deserializer))
            {
                cout << "Message received." << endl;
                if(auto init_message = dynamic_cast<systemV::message<message_type::INIT>*>(message.get()))
                {
                    handle_init_message(*init_message);
                }
                else if(auto list_message = dynamic_cast<::message<message_type::LIST_REQUEST>*>(message.get()))
                {
                    handle_list_message(*list_message);
                }
                else if(auto stop_message = dynamic_cast<::message<message_type::STOP>*>(message.get()))
                {
                    handle_stop_message(*stop_message);
                }
                else if(auto disconnect_message = dynamic_cast<::message<message_type::DISCONNECT>*>(message.get()))
                {
                    handle_disconnect_message(*disconnect_message);
                }
                else if(auto connect_message = dynamic_cast<::message<message_type::CONNECT_REQUEST>*>(message.get()))
                {
                    handle_connect_message(*connect_message);
                }
                else
                {
                    throw runtime_error("Unsupported message type received.");
                }
            }
        }

        handle_server_stop();
    }
    catch (const std::exception& e) {
        cout << e.what() << endl;
        return 1;
    }
}