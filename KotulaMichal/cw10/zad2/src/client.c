#include <common.h>
#include <messages.h>

int connect_inet(const char* ipv4, unsigned short port)
{
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ipv4, &addr.sin_addr) == -1) {
        printf("Invalid address\n");
        exit(EXIT_FAILURE);
    }

    int sock = exit_on_error(socket(AF_INET, SOCK_DGRAM, 0));
    exit_on_error(connect(sock, (struct sockaddr*) &addr, sizeof addr));

    return sock;
}

int connect_unix(const char* path, char* nick)
{
    struct sockaddr_un addr;
    struct sockaddr_un client_addr;

    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;
    client_addr.sun_family = AF_UNIX;

    snprintf(client_addr.sun_path, sizeof(client_addr.sun_path), "/tmp/%s%ld", nick, time(NULL));
    strncpy(addr.sun_path, path, sizeof(addr.sun_path));

    int sock = exit_on_error(socket(AF_UNIX, SOCK_DGRAM, 0));
    exit_on_error(bind(sock, (struct sockaddr*)&client_addr, sizeof(client_addr)));
    exit_on_error(connect(sock, (struct sockaddr*)&addr, sizeof(addr)));

    return sock;
}

struct client_state {
    char nicknames[2][MAX_NICK_LENGTH + 1];
    char symbol;
    bool my_turn;
    struct game_state game;
} client_state;

static const char board_format[] =
        ""
        "   %c │ %c │ %c\n"
        "  ───┼───┼───\n"
        "   %c │ %c │ %c \n"
        "  ───┼───┼───\n"
        "   %c │ %c │ %c\n";

void render_game()
{
    printf("\nYou: %s\n", client_state.nicknames[0]);
    printf("Other: %s\n\n", client_state.nicknames[1]);

    printf(board_format,
            client_state.game.board[0], client_state.game.board[1], client_state.game.board[2],
            client_state.game.board[3], client_state.game.board[4], client_state.game.board[5],
            client_state.game.board[6], client_state.game.board[7], client_state.game.board[8]
            );

    printf("Currently moving: %s\n", client_state.my_turn ? client_state.nicknames[0] : client_state.nicknames[1]);
}

define_handler(username_taken)
{
    printf("Username taken.\n");
    return true;
}

define_handler(server_full)
{
    printf("Server is full.\n");
    return true;
}

define_handler(ping)
{
    message reply = {.type = ping};
    send_message(sender_sock, reply);
    return false;
}

define_handler(start_game)
{
    client_state.symbol = msg->data.game_start.symbol;
    strncpy(client_state.nicknames[1], msg->data.game_start.nickname, MAX_NICK_LENGTH);
    client_state.my_turn = false;

    return false;
}

define_handler(state_changed)
{
    client_state.game = msg->data.state_changed;
    client_state.my_turn = client_state.game.move == client_state.symbol;

    render_game();

    return false;
}

void show_win_screen(char winner, int reason)
{
    printf("%s won%s\n",
            client_state.symbol == winner ? client_state.nicknames[0] : client_state.nicknames[1],
            reason == peer_disconnected ? " because other player disconnected" : "");
}

define_handler(game_ended)
{
    show_win_screen(msg->data.game_ended.winner, msg->data.game_ended.reason);

    return true;
}

define_handler(wait_for_opponent)
{
    printf("Waiting for opponent\n");
    return false;
}

bool stop = false;
int sock;

void terminal_interrupt(int s)
{
    stop = true;
    message msg = {.type = disconnect};
    send_message(sock, msg);
}

int main(int argc, char** argv)
{
    if (strcmp(argv[2], "web") == 0 && argc == 5) sock = connect_inet(argv[3], atoi(argv[4]));
    else if (strcmp(argv[2], "unix") == 0 && argc == 4) sock = connect_unix(argv[3], argv[1]);
    else {
        printf("zad1client [nickname] [web|unix] [ip port|path]\n");
        exit(EXIT_FAILURE);
    }

    strncpy(client_state.nicknames[0], argv[1], MAX_NICK_LENGTH);

    message msg = {.type = login};
    strncpy(msg.data.login.nickname, client_state.nicknames[0], MAX_NICK_LENGTH);

    send_message(sock, msg);

    int epoll_fd = exit_on_error(epoll_create1(0));

    struct epoll_event input_event =
    {
       EPOLLIN | EPOLLPRI,
       {.fd = STDIN_FILENO}
    };

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &input_event);

    struct epoll_event sock_event =
    {
        EPOLLIN | EPOLLPRI | EPOLLHUP,
        {.fd = sock}
    };

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &sock_event);

    signal(SIGINT, terminal_interrupt);

    while(!stop)
    {
        struct epoll_event events[2];
        int ready_to_read = epoll_wait(epoll_fd, events, 2, 1);

        for(int i=0;i<ready_to_read;i++)
        {
            if (events[i].data.fd == STDIN_FILENO) {
                int c;

                if (scanf("%d", &c) != 1) {  // if read int longer that 2 chars
                    char x;
                    while ((x = getchar()) != EOF && x != '\n'); // ignore invalid input
                    continue;
                }

                if (c < 1 || c > 9) continue;

                --c;

                if (client_state.my_turn) {
                    if (client_state.game.board[c] == ' ') { // if cell is empty
                        client_state.game.board[c] = client_state.symbol;
                        message msg = {.type = client_move};
                        msg.data.client_move = c;
                        send_message_to(sock, NULL, sizeof(struct sockaddr_in), msg);
                    }
                } else {
                    printf("Not your turn.\n");
                }
            } else {
                message msg;
                receive_message_from(sock, NULL, NULL, msg);

                bool error = false;

                if(events[i].events & EPOLLHUP)
                {
                    printf("Lost connection to server.\n");
                    error=true;
                } else{
                    switch (msg.type) {
                        handler_case_call(server_full, msg, sock, error)
                        handler_case_call(username_taken, msg, sock, error)
                        handler_case_call(ping, msg, sock, error)
                        handler_case_call(start_game, msg, sock, error)
                        handler_case_call(state_changed, msg, sock, error)
                        handler_case_call(game_ended, msg, sock, error);
                    }
                }


                if(error)
                {
                    stop = true;
                    break;
                }
            }
        }
    }

    close(epoll_fd);
    shutdown(sock, SHUT_RDWR);
    close(sock);

    return 0;
}