#pragma once

#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#define SERVER_KEY_PATHNAME "/home/michal/"
#define PROJECT_ID 'S'

#define MAX_CLIENTS_COUNT 10
#define QUEUE_PERMISSIONS 0640

typedef int queue_id_t;
typedef int host_id_t;
typedef enum message_type
{
    STOP = 1,
    DISCONNECT = 2,
    LIST_REQUEST = 3,
    LIST_REPLY = 4,
    ERROR = 5,
    CONNECT_REQUEST = 6,
    CONNECT_REPLY = 7,
    INIT = 8,
    INIT_REPLY = 9
} message_type;

typedef enum error_code {
    ALREADY_CONNECTED = -3,
    SERVER_FULL = -2,
    INVALID_CLIENT_ID = -1,
    OK = 0
} error_code;

typedef struct message
{
    long mtype;
    union data{
        struct common
        {
            host_id_t id;
        } disconnect, stop, list_request, init_reply;
        struct connect_reply
        {
            queue_id_t queue_id;
        } init, connect_reply;
        struct connect_request
        {
            host_id_t id;
            host_id_t id_to;
        } connect_request;
        struct error
        {
            enum error_code ec;
        } error;
        struct list_reply {
            int size;
            struct { host_id_t id; bool available;} clients[MAX_CLIENTS_COUNT];
        } list_reply;
    } data;
} message;

#define send_message(type, initializer, queue) \
{ \
    message msg = {type, {initializer}}; \
    msgsnd(queue, &msg, sizeof(msg), 0); \
}
#define peek_message(queue, msg) \
(msgrcv(queue, &msg, sizeof(msg), -20, IPC_NOWAIT) != -1)
#define receive_message(queue, msg) \
(msgrcv(queue, &msg, sizeof(msg), -20, 0) != -1)
#define define_handler(msg_type) \
error_code msg_type##_handler(message* msg)
#define handler_case_call(msg_type, msg, error) \
    case msg_type:\
        error = msg_type##_handler(&msg);\
        break;
