#pragma once

#include <stdbool.h>
#include <mqueue.h>
#include <sys/types.h>
#include <time.h>

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#define SERVER_KEY {"/srv"};

#define MAX_CLIENTS_COUNT 10
#define QUEUE_PERMISSIONS 0666
#define MAX_TEXT_LENGTH 128

typedef union{
    char code[5];
} queue_id_t;
typedef mqd_t queue_t;
typedef int host_id_t;
typedef enum message_type
{
    STOP = 1,
    STOP_REPLY = 2,
    DISCONNECT = 3,
    LIST_REQUEST = 4,
    LIST_REPLY = 5,
    ERROR = 6,
    CONNECT_REQUEST = 7,
    CONNECT_REPLY = 8,
    INIT = 9,
    INIT_REPLY = 10,
    CHAT = 11
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
        } disconnect, stop, stop_reply, list_request, init_reply;
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
        struct chat {
            char text[MAX_TEXT_LENGTH];
        } chat;
    } data;
} message;

#define send_message(type, initializer, queue) \
{ \
    message msg = {type, {initializer}}; \
    int v = mq_send(queue, (char*)&msg, sizeof(msg), type);\
}
#define receive_message(queue, msg) \
(mq_receive(queue, (char*)&msg, sizeof(msg), NULL) != -1)
#define define_handler(msg_type) \
error_code msg_type##_handler(message* msg)
#define handler_case_call(msg_type, msg, error) \
    case msg_type:\
        error = msg_type##_handler(&msg);\
        break;
