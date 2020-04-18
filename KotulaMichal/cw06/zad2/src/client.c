#include <common/common.h>

queue_id_t server_queue_id = SERVER_KEY;
queue_id_t client_queue_id;
queue_t client_queue;
queue_t server_queue;
host_id_t client_id;
queue_t other_queue = -1;
bool closed = false;

define_handler(INIT_REPLY)
{
    printf("[Client] Connected to server\n");
    client_id = msg->data.init_reply.id;
    return 0;
}

define_handler(LIST_REPLY)
{
    printf("[Client] Available clients:\n");
    for(size_t i=0;i<msg->data.list_reply.size; i++)
    {
        printf("Client %i: %s\n",
               msg->data.list_reply.clients[i].id,
               msg->data.list_reply.clients[i].available ? "available" : "not available");
    }
    return OK;
}

define_handler(DISCONNECT)
{
    other_queue = -1;
    printf("[Client] Other client disconnected\n");
    return OK;
}

define_handler(CHAT)
{
    printf("[Client] Other says: %s\n", msg->data.chat.text);
    return OK;
}

define_handler(STOP)
{
    printf("[Client] Server stopped\n");
    send_message(STOP_REPLY, {0}, server_queue);
    mq_close(server_queue);
    mq_close(client_queue);
    mq_unlink(client_queue_id.code);
    closed=true;
    exit(0);
    return OK;
}

define_handler(CONNECT_REPLY)
{
    other_queue = mq_open(msg->data.connect_reply.queue_id.code, O_WRONLY);
    printf("[Client] Connected with target client through queue: %i\n", other_queue);
    return OK;
}

define_handler(ERROR)
{
    switch(msg->data.error.ec)
    {
        case ALREADY_CONNECTED:
            printf("[Client] Target is already connected\n");
            break;
        case INVALID_CLIENT_ID:
            printf("[Client] Invalid target id\n");
            break;
        case SERVER_FULL:
            printf("[Client] Server is full\n");
            break;
    }
    return OK;
}

void register_message_handler();

void on_message_available(union sigval sv)
{
    (void) sv;

    register_message_handler();

    message msg;

    while(receive_message(client_queue, msg))
    {
        error_code error;
        switch(msg.mtype)
        {
            handler_case_call(STOP, msg, error)
            handler_case_call(CONNECT_REPLY, msg, error)
            handler_case_call(LIST_REPLY, msg, error)
            handler_case_call(DISCONNECT, msg, error)
            handler_case_call(CHAT, msg, error)
            handler_case_call(ERROR, msg, error)
        }
    }
}

void register_message_handler()
{
    struct sigevent event;

    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = on_message_available;
    event.sigev_notify_attributes = NULL;
    event.sigev_value.sival_ptr = NULL;

    mq_notify(client_queue, &event);
}

void onexit()
{
    if(!closed)
    {
        send_message(STOP, {client_id}, server_queue);
        message msg;
        while(receive_message(client_queue, msg))
        {
            if(msg.mtype == STOP_REPLY)
            {
                break;
            }
        }
        mq_close(server_queue);
        mq_close(client_queue);
        mq_unlink(client_queue_id.code);
    }
}

int main()
{
    srand(time(NULL));

    server_queue = mq_open(server_queue_id.code, O_WRONLY);
    if(server_queue == -1)
    {
        printf("Failed to get server queue");
        return 0;
    }

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(message);
    attr.mq_curmsgs = 0;

    client_queue_id.code[0] = '/';
    client_queue_id.code[1] = rand() % ('z' - 'a') + 'a';
    client_queue_id.code[2] = rand() % ('z' - 'a') + 'a';
    client_queue_id.code[3] = rand() % ('z' - 'a') + 'a';
    client_queue_id.code[4] = '\0';

    client_queue = mq_open(client_queue_id.code, O_CREAT | O_EXCL | O_RDONLY, QUEUE_PERMISSIONS, &attr);
    if(server_queue == -1)
    {
        printf("Failed to get client queue");
        return 0;
    }

    message msg = {INIT};
    msg.data.init.queue_id = client_queue_id;
    mq_send(server_queue, (char*)&msg, sizeof(msg), msg.mtype);
    if(receive_message(client_queue, msg))
    {
        error_code error;
        switch(msg.mtype)
        {
            handler_case_call(INIT_REPLY, msg, error)
            handler_case_call(ERROR, msg, error)
        }

        if(client_id == 0)
        {
            exit(0);
        }
    }

    atexit(onexit);
    signal(SIGINT, exit);

    mq_getattr(client_queue, &attr);
    attr.mq_flags = O_NONBLOCK;
    mq_setattr(client_queue, &attr, NULL);

    register_message_handler();

    while(true)
    {
        char command[MAX_TEXT_LENGTH];
        scanf("%s", command);

        if(strcmp(command, "LIST") == 0)
        {
            send_message(LIST_REQUEST, {client_id}, server_queue);
        }
        else if(strcmp(command, "CONNECT") == 0)
        {
            if(other_queue != -1)
            {
                printf("[Client] You are already connected to someone\n");
            } else{
                host_id_t other_id;
                scanf("%i", &other_id);

                message msg = {CONNECT_REQUEST, {}};
                msg.data.connect_request.id = client_id;
                msg.data.connect_request.id_to = other_id;
                mq_send(server_queue, (char*)&msg, sizeof(msg), msg.mtype);
            }
        }
        else if(strcmp(command, "DISCONNECT") == 0)
        {
            if(other_queue != -1)
            {
                send_message(DISCONNECT, {client_id}, server_queue);
            } else{
                printf("[Client] You are not connected to anyone\n");
            }
        }
        else if(strcmp(command, "STOP") == 0)
        {
            exit(0);
        } else{ // chat message
            if(other_queue != -1)
            {
                message msg = {CHAT};
                strcpy(msg.data.chat.text, command);
                mq_send(other_queue, (char*)&msg, sizeof(msg), msg.mtype);
            } else{
                printf("[Client] You are not connected to anyone\n");
            }
        }
    }
}