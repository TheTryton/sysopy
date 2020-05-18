#include <common.h>
#include <messages.h>
#include <pthread.h>

#define MAX_CLIENTS 32

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int epoll_fd;

typedef struct client {
    int sock;
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

typedef struct event_data {
    event_type type;
    union data {
        client* client;
        int socket;
    } data;
} event_data;

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

        send_message(c->peer->sock, win);

        c->peer->peer = NULL;

        delete_client(c->peer);
    }

    if(c == waiting_client)
    {
        waiting_client = NULL;
    }

    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, c->sock, NULL);

    shutdown(c->sock, SHUT_RDWR);
    close(c->sock);

    c->sock = 0;
    c->state = null;
    c->peer = NULL;
    c->symbol = 0;
    c->responding = false;

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

    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, c->sock, NULL);

    shutdown(c->sock, SHUT_RDWR);
    close(c->sock);

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
            send_message(sender->sock, msg);

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
        send_message(waiting_client->sock, msg);

        msg.data.game_start.symbol = sender->symbol;
        strncpy(msg.data.game_start.nickname, waiting_client->nickname, MAX_NICK_LENGTH);
        send_message(sender->sock, msg);

        msg.type = state_changed;
        msg.data.state_changed = *state;

        send_message(waiting_client->sock, msg);
        send_message(sender->sock, msg);

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

    send_message(sender->sock, reply);
    send_message(sender->peer->sock, reply);

    char c = check_win(sender->game_state->board);
    if(c == sender->symbol || c == sender->peer->symbol)
    {
        message win = {.type = game_ended};
        win.data.game_ended.reason = game_won;
        win.data.game_ended.winner = c;

        send_message(sender->sock, win);
        send_message(sender->peer->sock, win);

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

void bind_listen_socket(int socket, void* addr, int addr_size) {
    exit_on_error(bind(socket, (struct sockaddr*) addr, addr_size));
    exit_on_error (listen(socket, MAX_CLIENTS));

    event_data* client_to_accept = calloc(1, sizeof(event_data));
    client_to_accept->type = server_socket_event;
    client_to_accept->data.socket = socket;

    struct epoll_event event =
    {
        .events = EPOLLIN | EPOLLPRI,
        .data = { .ptr = client_to_accept }
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

                    send_message(clients[i].sock, msg);
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

    int unix_sock = exit_on_error(socket(AF_UNIX, SOCK_STREAM, 0));
    bind_listen_socket(unix_sock, &server_unix_address, sizeof(server_unix_address));

    int inet_sock = exit_on_error(socket(AF_INET, SOCK_STREAM, 0));
    bind_listen_socket(inet_sock, &server_inet_address, sizeof(server_inet_address));

    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, do_ping, NULL);

    signal(SIGINT, terminal_interrupt);

    while(!stop) {
        struct epoll_event events[MAX_CLIENTS+2];
        int ready_to_read = exit_on_error(epoll_wait(epoll_fd, events, MAX_CLIENTS+2, -1));

        for(int i=0;i<ready_to_read;i++) {

            event_data* data = events[i].data.ptr;

            if (data->type == server_socket_event) {
                // new client available
                int client_sock = accept(data->data.socket, NULL, NULL);

                client* client = add_client(client_sock);

                if (client == NULL) {
                    message msg = { .type = server_full };
                    send_message(client_sock, msg);
                    shutdown(client_sock, SHUT_RDWR);
                    close(client_sock);
                } else{
                    event_data* client_written_something = calloc(1, sizeof(event_data));
                    client_written_something->type = client_socket_event;
                    client_written_something->data.client = client;

                    struct epoll_event event = {
                        .events = EPOLLIN | EPOLLET | EPOLLHUP,
                        .data = { .ptr = client_written_something }
                    };

                    exit_on_error(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &event));
                }

            } else if (data->type == client_socket_event) {
                message msg;
                receive_message(data->data.client->sock, msg);

                bool error = false;

                if(events[i].events & EPOLLHUP)
                {
                    pthread_mutex_lock(&mutex);
                    delete_client(data->data.client);
                    pthread_mutex_unlock(&mutex);
                } else{
                    switch (msg.type) {
                        server_handler_case_call(login, msg, data->data.client, error)
                        server_handler_case_call(client_move, msg, data->data.client, error)
                        server_handler_case_call(ping, msg, data->data.client, error)
                        server_handler_case_call(disconnect, msg, data->data.client, error)
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