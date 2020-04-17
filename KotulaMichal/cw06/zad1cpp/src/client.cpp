#include <common/message/queue/systemV/queue.hpp>
#include <common/message/deserializer/systemV/deserializer.hpp>
#include <unistd.h>
#include <cstring>

using namespace std;

unique_ptr<imessage_queue> client_queue;
unique_ptr<imessage_queue> server_queue;
unique_ptr<imessage_deserializer> message_deserializer;
client_id_t client_id;
bool stopped = false;

struct other_client_info
{
    client_id_t other_client_id;
    key_t other_client_queue_key;
    unique_ptr<imessage_queue> other_client_queue;
};

optional<other_client_info> other_client;

void send_init_message()
{
    server_queue->send_message(systemV::message<message_type::INIT>(static_cast<systemV::message_queue*>(client_queue.get())->key()));

    if(auto msg = client_queue->receive_message(*message_deserializer))
    {
        if(auto init_reply_msg = dynamic_cast<message<message_type::INIT_REPLY>*>(msg.get()))
        {
            client_id = init_reply_msg->assigned_id();
            cout << "Server assigned id: " << client_id << endl;
        } else{
            throw runtime_error("Received message is not of INIT_REPLY type");
        }
    }
}

void send_list_message()
{
    server_queue->send_message(message<message_type::LIST_REQUEST>(client_id));

    if(auto msg = client_queue->receive_message(*message_deserializer))
    {
        if(auto list_reply_msg = dynamic_cast<message<message_type::LIST_REPLY>*>(msg.get()))
        {
            for(auto& client : list_reply_msg->listed_clients())
            {
                cout << "Client " << client.first;
                if(client.second)
                {
                    cout << " is connected to " << *client.second << "." << endl;
                }
                else
                {
                    cout << " is available for connection." << endl;
                }
            }
        } else{
            throw runtime_error("Received message is not of LIST_REPLY type");
        }
    }
}

void send_stop_message()
{
    server_queue->send_message(message<message_type::STOP>(client_id));
    stopped=true;
}

void send_connect_message(client_id_t other_client_id)
{
    server_queue->send_message(message<message_type::CONNECT_REQUEST>(client_id, other_client_id));
    if(auto msg = client_queue->receive_message(*message_deserializer))
    {
        if(auto connect_reply_msg = dynamic_cast<systemV::message<message_type::CONNECT_REPLY>*>(msg.get()))
        {
            if(connect_reply_msg->target_key())
            {
                other_client =
                {
                    other_client_id,
                    *connect_reply_msg->target_key(),
                    unique_ptr<imessage_queue>(new systemV::message_queue(*connect_reply_msg->target_key()))
                };
            }
            else
            {
                cout << "Client is already connected." << endl;
            }
        } else{
            throw runtime_error("Received message is not of CONNECT_REPLY type");
        }
    }
}

void send_disconnect_message()
{
    server_queue->send_message(message<message_type::DISCONNECT>(client_id));
    other_client = nullopt;
}

void send_chat_message(const string& text)
{
    cout << "You said: " << text << endl;
    other_client->other_client_queue->send_message(message<message_type::CHAT>(client_id, text));
}

void handle_disconnect_message()
{
    other_client = nullopt;
}

void handle_chat_message(const message<message_type::CHAT>& msg)
{
    cout << "Other client says: " << msg.text() << endl;
}

void handle_connect_reply_message(const systemV::message<message_type::CONNECT_REPLY>& msg)
{
    other_client =
            {
                    msg.sender_id(),
                    *msg.target_key(),
                    unique_ptr<imessage_queue>(new systemV::message_queue(*msg.target_key()))
            };
}

void handle_terminal_interrupt_signal(int signum) {
    send_stop_message();
};

int main(int argc, char** argv)
{
    signal(SIGINT, handle_terminal_interrupt_signal);
    srand(time(NULL));
    auto client_key_path = "/home/michal/Desktop/operating_systems/KotulaMichal/cw06/zad1/";
    auto project_id = rand()%256;

    cout << "Provide server message queue key: ";
    key_t server_queue_key;
    cin >> server_queue_key;
    try{
        client_queue = unique_ptr<imessage_queue>(new systemV::message_queue(client_key_path, project_id));
        cout << "Client message_queue key: " << static_cast<systemV::message_queue*>(client_queue.get())->key() << endl;
        message_deserializer = unique_ptr<imessage_deserializer>(new systemV::message_deserializer());

        server_queue = unique_ptr<imessage_queue>(new systemV::message_queue(server_queue_key));
        cout << "Connected to server." << endl;

        send_init_message();

        // now i'm setting terminal mode to be non blocking

        while(!stopped)
        {
            string input;
            cin >> input;

            if(!other_client)
            {
                if(input == "LIST")
                {
                    send_list_message();
                }
                else if(input == "STOP")
                {
                    send_stop_message();
                }
                else if(input == "CONNECT")
                {
                    client_id_t other_client_id;
                    cin >> other_client_id;
                    send_connect_message(other_client_id);
                }
            } else{
                if(input == "LIST")
                {
                    send_list_message();
                }
                else if(input == "STOP")
                {
                    send_disconnect_message();
                    send_stop_message();
                }
                else if(input == "DISCONNECT")
                {
                    send_disconnect_message();
                } else{
                    send_chat_message(input);
                }
            }

            while(auto message = client_queue->peek_message(*message_deserializer))
            {
                if(dynamic_cast<::message<message_type::DISCONNECT>*>(message.get()))
                {
                    handle_disconnect_message();
                } else if(auto chat_message = dynamic_cast<::message<message_type::CHAT>*>(message.get())) {
                    handle_chat_message(*chat_message);
                }
                else if(auto connect_reply_message = dynamic_cast<systemV::message<message_type::CONNECT_REPLY>*>(message.get())) {
                    handle_connect_reply_message(*connect_reply_message);
                }
            }
        }
    }
    catch (const std::exception& e) {
        cout << e.what() << endl;

        if(server_queue && !stopped)
        {
            send_stop_message();
        }

        return 1;
    }
}