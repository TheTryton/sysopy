#pragma once

#define MAX_NICK_LENGTH 32

typedef struct game_state {
    char move;
    char board[9];
} game_state;

typedef enum message_type {
    login, start_game, client_move,
    state_changed, game_ended,
    ping, username_taken,
    server_full, wait_for_opponent,
    disconnect, server_stopped
} message_type;

typedef struct message {
    message_type type;
    union message_data {
        struct { char nickname[MAX_NICK_LENGTH + 1]; } login;
        struct { char nickname[MAX_NICK_LENGTH + 1]; char symbol; } game_start;
        int client_move;
        game_state state_changed;
        struct { enum reason { game_won, peer_disconnected} reason; char winner; } game_ended;
    } data;
} message;

#define define_handler(msg_type) \
bool msg_type##_handler(message* msg, int sender_sock)

#define define_server_handler(msg_type) \
bool msg_type##_server_handler(message* msg, client* sender)

#define handler_case_call(msg_type, msg, sender_sock, error) \
    case msg_type: \
        error = msg_type##_handler(&msg, sender_sock);\
        break;

#define server_handler_case_call(msg_type, msg, sender_client, error) \
    case msg_type: \
        error = msg_type##_server_handler(&msg, sender_client);\
        break;

#define send_message(sock, message) \
    send(sock, (void*)&message, sizeof(message), 0);

#define send_message_to(sock, to, to_len, message) \
    sendto(sock, (void*)&message, sizeof(message), 0, (struct sockaddr*)to, to_len);

#define receive_message_from(sock, from, from_len, message) \
    recvfrom(sock, (void*)&message, sizeof(message), 0, (struct sockaddr*)from, from_len);
