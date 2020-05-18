#include <common.h>
#include <messages.h>
#include <pthread.h>

#define MAX_CLIENTS 32

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int epoll_fd;

typedef union client_address
{
    struct sockaddr_un _unix;
    struct sockaddr_in _inet;
} client_address;

typedef struct client {
    client_address address;
    int sock, address_length;
    char nickname[MAX_NICK_LENGTH + 1];

    enum client_state { null, wait_for_login, waiting, playing } state;
    struct client* peer;

    char symbol;
    struct game_state* game_state;

    bool responding;
} client;

client clients[MAX_CLIENTS], *waiting_client = NULL;

typedef struct client client;

typedef enum event_type { server_socket_event, client_socket_event } event_type;

client* add_client(int sock)
{
    pthread_mutex_lock(&mutex);
    for(size_t i=0; i < MAX_CLIENTS; i++)
    {
        if(clients[i].state == null)
        {
            clients[i].state = wait_for_login;
            clients[i].sock = sock;
            clients[i].responding = true;
            pthread_mutex_unlock(&mutex);
            return &clients[i];
        }
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void delete_client(client* c)
{
    if(c->peer)
    {
        free(c->game_state);
        c->peer->game_state = NULL;
        c->game_state = NULL;

        message win = {.type = game_ended};
        win.data.game_ended.reason = peer_disconnected;
        win.data.game_ended.winner = c->peer->symbol;

        send_message_to(c->peer->sock,&c->peer->address, c->peer->address_length, win);

        c->peer->peer = NULL;

        delete_client(c->peer);
    }

    if(c == waiting_client)
    {
        waiting_client = NULL;
    }

    c->sock = 0;
    c->state = null;
    c->peer = NULL;
    c->symbol = 0;
    c->responding = false;

    memset(&c->address, 0, sizeof(c->address));

    printf("Client %s disconnected.\n", c->nickname);
}

void delete_client_no_recurr(client* c)
{
    if(c->peer)
    {
        free(c->game_state);
        c->peer->game_state = NULL;
        c->game_state = NULL;

        c->peer->peer = NULL;
    }

    if(c == waiting_client)
    {
        waiting_client = NULL;
    }

    c->sock = 0;
    c->state = null;
    c->peer = NULL;
    c->symbol = 0;
    c->responding = false;

    printf("Client %s disconnected.\n", c->nickname);
}

define_server_handler(login)
{
    pthread_mutex_lock(&mutex);

    printf("Client %s connected.\n", msg->data.login.nickname);

    for(size_t i=0; i<MAX_CLIENTS; i++)
    {
        if(clients[i].state != null && clients[i].state != wait_for_login && strcmp(clients[i].nickname, msg->data.login.nickname) == 0)
        {
            printf("Username %s taken.\n", msg->data.login.nickname);

            message msg = { .type = username_taken };
            send_message_to(sender->sock, &sender->address, sender->address_length, msg);

            delete_client_no_recurr(sender);

            pthread_mutex_unlock(&mutex);
            return false;
        }
    }

    strncpy(sender->nickname, msg->data.login.nickname, MAX_NICK_LENGTH);

    if(waiting_client != NULL) // connect them and prepare game
    {
        waiting_client->peer = sender;
        sender->peer = waiting_client;

        waiting_client->symbol = 'X';
        sender->symbol = 'O';

        game_state* state = calloc(1, sizeof(game_state));
        memset(state->board, ' ', sizeof(state->board));
        state->move = waiting_client->symbol;

        waiting_client->game_state = state;
        sender->game_state = state;

        message msg = {.type = start_game};

        msg.data.game_start.symbol = waiting_client->symbol;
        strncpy(msg.data.game_start.nickname, sender->nickname, MAX_NICK_LENGTH);
        send_message_to(waiting_client->sock, &waiting_client->address, waiting_client->address_length, msg);

        msg.data.game_start.symbol = sender->symbol;
        strncpy(msg.data.game_start.nickname, waiting_client->nickname, MAX_NICK_LENGTH);
        send_message_to(sender->sock, &sender->address, sender->address_length, msg);

        msg.type = state_changed;
        msg.data.state_changed = *state;

        send_message_to(waiting_client->sock, &waiting_client->address, waiting_client->address_length, msg);
        send_message_to(sender->sock, &sender->address, sender->address_length, msg);

        waiting_client->state = playing;
        sender->state = playing;

        waiting_client = NULL;
    } else{
        waiting_client = sender;
        sender->state = waiting;
        message reply = {.type = wait_for_opponent};
        send_message(sender->sock, reply);
    }

    pthread_mutex_unlock(&mutex);

    return false;
}

char check_win(char board[9])
{
    static size_t win_conditions[8][3] = {
            {0,1,2},{3,4,5},{6,7,8}, //horizontal
            {0,3,6},{1,4,7},{2,5,8}, //vertical
            {0,4,8},{2,4,6} //diagonal
    };

    for(size_t i=0;i<8;i++)
    {
        char symbol = board[win_conditions[i][0]];

        if(board[win_conditions[i][1]] == symbol && board[win_conditions[i][2]] == symbol)
        {
            return symbol;
        }
    }

    return 0;
}

define_server_handler(client_move)
{
    pthread_mutex_lock(&mutex);

    sender->game_state->board[msg->data.client_move] = sender->symbol;
    sender->game_state->move = sender->peer->symbol;

    message reply = {.type = state_changed};

    reply.data.state_changed = *sender->game_state;

    send_message_to(sender->sock, &sender->address, sender->address_length, reply);
    send_message_to(sender->peer->sock, &sender->peer->address, sender->peer->address_length, reply);

    char c = check_win(sender->game_state->board);
    if(c == sender->symbol || c == sender->peer->symbol)
    {
        message win = {.type = game_ended};
        win.data.game_ended.reason = game_won;
        win.data.game_ended.winner = c;

        send_message_to(sender->sock, &sender->address, sender->address_length, win);
        send_message_to(sender->peer->sock, &sender->peer->address, sender->peer->address_length, win);

        delete_client_no_recurr(sender->peer);
        delete_client_no_recurr(sender);
    }

    pthread_mutex_unlock(&mutex);

    return false;
}

define_server_handler(ping)
{
    pthread_mutex_lock(&mutex);

    sender->responding = true;

    pthread_mutex_unlock(&mutex);

    return false;
}

define_server_handler(disconnect)
{
    pthread_mutex_lock(&mutex);

    delete_client(sender);

    pthread_mutex_unlock(&mutex);

    return false;
}

void bind_socket(int socket, void* addr, int addr_size) {
    exit_on_error(bind(socket, (struct sockaddr*) addr, addr_size));

    struct epoll_event event =
    {
        .events = EPOLLIN | EPOLLPRI,
        .data = { .fd = socket }
    };

    exit_on_error(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket, &event));
}

void* do_ping(void* arg) {
    static message msg = { .type = ping };

    while(true){
        sleep(2);
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].state != null) {
                if (clients[i].responding) {
                    clients[i].responding = false;

                    send_message_to(clients[i].sock, &clients[i].address, clients[i].address_length, msg);
                }
                else
                {
                    delete_client(&clients[i]);
                }
            }
        }
        pthread_mutex_unlock(&mutex);
    }
}

bool stop = false;

void terminal_interrupt(int s)
{
    stop = true;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("zad1server [port] [path]\n");
        exit(0);
    }
    int port = atoi(argv[1]);
    char* socket_path = argv[2];

    epoll_fd = exit_on_error(epoll_create1(0));

    struct sockaddr_un server_unix_address = { .sun_family = AF_UNIX };
    strncpy(server_unix_address.sun_path, socket_path, sizeof(server_unix_address.sun_path));

    unlink(socket_path);

    struct sockaddr_in server_inet_address = {
            .sin_family = AF_INET, .sin_port = htons(port),
            .sin_addr = { .s_addr = htonl(INADDR_ANY) },
    };

    int unix_sock = exit_on_error(socket(AF_UNIX, SOCK_DGRAM, 0));
    bind_socket(unix_sock, &server_unix_address, sizeof(server_unix_address));

    int inet_sock = exit_on_error(socket(AF_INET, SOCK_DGRAM, 0));
    bind_socket(inet_sock, &server_inet_address, sizeof(server_inet_address));

    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, do_ping, NULL);

    signal(SIGINT, terminal_interrupt);

    while(!stop) {
        struct epoll_event events[MAX_CLIENTS+2];
        int ready_to_read = exit_on_error(epoll_wait(epoll_fd, events, MAX_CLIENTS+2, -1));

        for(int i=0;i<ready_to_read;i++) {
            int socket = events[i].data.fd;

            message msg;

            client_address address;
            socklen_t address_length = sizeof(address);
            receive_message_from(socket, &address, &address_length, msg);

            if(msg.type == login)
            {
                client* client = add_client(socket);
                pthread_mutex_lock(&mutex);
                client->address = address;
                client->address_length = address_length;
                pthread_mutex_unlock(&mutex);
                if (client == NULL) {
                    message msg = { .type = server_full };
                    send_message_to(socket, &address, address_length, msg);
                } else{
                    login_server_handler(&msg, client);
                }

            } else{
                client* client = NULL;

                for(size_t i=0;i<MAX_CLIENTS;i++)
                {
                    if(memcmp(&clients[i].address, &address, address_length) == 0)
                    {
                        client = &clients[i];
                        break;
                    }
                }

                if(!client)
                {
                    printf("Can't find client.\n");
                } else{
                    bool error = false;
                    switch (msg.type) {
                        server_handler_case_call(client_move, msg, client, error)
                        server_handler_case_call(ping, msg, client, error)
                        server_handler_case_call(disconnect, msg, client, error)
                    }
                }
            }
        }
    }

    message msg = {.type = server_stopped};
    for(size_t i=0;i<MAX_CLIENTS;i++)
    {
        if(clients[i].state != null)
        {
            send_message(clients[i].sock, msg);
            delete_client(&clients[i]);
        }
    }
}