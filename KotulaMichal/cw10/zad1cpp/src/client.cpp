#include <sockets/all.hpp>
#include <iostream>

using std::cout;
using std::endl;
using std::thread;
using namespace sockets;
using namespace sockets::ip;

class client
{
private:
    io_context& _context;
    tcp::connector _connector;
    tcp::socket _server;
    string _name;
    int _assigned_id;
public:
    client(io_context& context, string_view name, const tcp::endpoint& server_endpoint)
        : _context(context)
        , _connector(context)
        , _server(context)
        , _name(name)
    {
        cout << "[Client] Connecting to server." << endl;
        do_connect(server_endpoint);
    }
private:
    void do_connect(const tcp::endpoint& server_endpoint)
    {
        _connector.async_connect(_server, server_endpoint, [this](const tcp::endpoint& ep, error_code& ec){
            if(ec != error_code())
            {
                cout << ec.message() << endl;
                _context.stop();
                return;
            }

            cout << "[Client] Connected to server." << endl;
            cout << "[Client] Sending login message." << endl;

            do_login();
        });
    }

    void do_login()
    {
        login_message msg;
        msg.client_name = _name;
        msg_header login_header = msg.header();

        error_code ec;
        _server.write(reinterpret_cast<byte*>(&login_header), sizeof(login_header), ec);
        _server.write(reinterpret_cast<byte*>(msg.client_name.data()), msg.client_name.size(), ec);

        cout << "[Client] Sent login message." << endl;
        cout << "[Client] Waiting for authorization." << endl;

        do_wait_for_authorization();
    }

    void do_wait_for_authorization()
    {
        authorization_result result;
        msg_header header;

        error_code ec;
        _server.read(reinterpret_cast<byte*>(&header), sizeof(header), ec);
        if(header.msg_type != message_type::authorization_result)
        {
            cout << "[Client] Invalid response received from server, stopping." << endl;
            _context.stop();
        } else{
            _server.read(reinterpret_cast<byte*>(&result), sizeof(result), ec);
            if(!result.failed)
            {
                cout << "[Client] Succesfully connected to server, assigned id: " << result.assigned_id << "." << endl;
                _assigned_id = result.assigned_id;
                do_read();
            } else{
                cout << "[Client] Login rejected (duplicate name), stopping." << endl;
                _context.stop();
            }
        }
    }

    void do_read(){}
};

int main(int argc, char** argv)
{
    /*if(argc != 3)
    {
        printf("zad1 tcp_udp_port unix_socket\n");
        return 0;
    }*/

    uint16_t port = 60000;//atoi(argv[1]);
    string_view server_address = "10.0.2.15";
    string_view name = "Gosc";

    io_context context;

    client c{context, name, tcp::endpoint(address_v4::from_string(server_address), port)};

    thread t([&](){
        context.run();
    });

    t.join();

    return 0;
}