#include <common.hpp>

class client_server_communication
{
private:
    shared_ptr<tcp::socket> _socket;
    int _client_id;
    string _client_name;
    size_t _msg_size;
    vector<byte> _msg_data;
    function<void(const vector<byte>&, int)> _on_msg_received;
public:
    client_server_communication(string client_name, int client_id, const shared_ptr<tcp::socket>& socket, function<void(const vector<byte>&, int)> on_msg_received)
        : _socket(socket)
        , _on_msg_received(on_msg_received)
        , _client_id(client_id)
        , _client_name(client_name)
    {
        do_read_header();
    }
private:
    void do_read_header()
    {
        _socket->async_read(reinterpret_cast<byte*>(&_msg_size), sizeof(_msg_size), [&](size_t,error_code& ec){
            if(ec != error_code())
            {
                cout << ec.message() << endl;
                do_read_header();
            }
            else
            {
                do_read();
            }
        });
    }
    void do_read()
    {
        _msg_data.resize(_msg_size);
        _socket->async_read(reinterpret_cast<byte*>(_msg_data.data()), _msg_data.size(), [&](size_t,error_code& ec){
            if(ec != error_code())
            {
                cout << ec.message() << endl;
            }
            else
            {
                _on_msg_received(_msg_data, _client_id);
            }

            do_read_header();
        });
    }
};

class server
{
private:
    io_context& _context;
    map<string, std::shared_ptr<client_server_communication>> _clients;
    int _curr_id = 0;
    shared_ptr<tcp::socket> _new_client;
    tcp::acceptor _acceptor;
public:
    server(io_context& context, const tcp::endpoint& endpoint)
        : _context(context)
        , _acceptor(_context, endpoint)
    {
        cout << "[Server] Waiting for clients." << endl;
        do_accept();
    }
public:
    void do_accept()
    {
        _new_client = std::make_shared<tcp::socket>(_context);
        _acceptor.async_accept(*_new_client, [&](error_code& ec){
            if(ec != error_code())
            {
                cout << ec.message() << endl;
            }
            else
            {
                do_authorize_client();
            }

            do_accept();
        });
    }

    void do_authorize_client()
    {
        cout << "[Server] Client connected." << endl;
        cout << "[Server] From: " << _new_client->socket_address().to_string() << endl;
        cout << "[Server] Waiting for client name." << endl;
        msg_header header;
        error_code ec;
        _new_client->read(reinterpret_cast<byte*>(&header),sizeof(header), ec);

        if(header.msg_type == message_type::login)
        {
            login_message login_msg;
            login_msg.client_name.resize(header.msg_size);

            _new_client->read(reinterpret_cast<byte*>(login_msg.client_name.data()),header.msg_size, ec);

            cout << "[Server] Name received: " <<  login_msg.client_name << "." << endl;

            authorization_result response;
            msg_header header = response.header();

            if(_clients.find(login_msg.client_name) != _clients.end())
            {
                cout << "[Server] Name available." << endl;

                response.assigned_id = _curr_id;
                response.failed = false;

                cout << "[Server] Assigned id: " << _curr_id << endl;

                _clients[login_msg.client_name] = std::make_shared<client_server_communication>(
                        login_msg.client_name,
                        _curr_id,
                        _new_client,
                        [&](const vector<byte>& data, int client_id){ handle_client_message(data,client_id);}
                );

                _curr_id++;

            } else{
                cout << "[Server] Name unavailable, disconnecting client." << endl;
                response.failed=true;
            }

            _new_client->write(reinterpret_cast<byte*>(&header),sizeof(header), ec);
            _new_client->write(reinterpret_cast<byte*>(&response),sizeof(response), ec);
        } else{
            cout << "[Server] Invalid message received from client, expected login message." << endl;
        }

        _new_client = std::make_shared<tcp::socket>(_context);
    }

    void handle_client_message(const vector<byte>& data, int client_id)
    {
        string data_vis;
        data_vis.resize(data.size());
        std::transform(data.begin(), data.end(), data_vis.begin(), [](const byte& b){
            return static_cast<char>(b);
        });
        cout << data_vis << " from " << client_id << endl;
    }
};

int main(int argc, char** argv)
{
    /*if(argc != 3)
    {
        printf("zad1 tcp_udp_port unix_socket\n");
        return 0;
    }*/

    uint16_t port = 60000;//atoi(argv[1]);

    io_context context;

    server s{context, tcp::endpoint(tcp::v4(),port)};

    thread t([&](){
        context.run();
    });

    t.join();

    return 0;
}