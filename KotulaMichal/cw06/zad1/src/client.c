#include <common/common.h>

queue_id_t client_queue;
queue_id_t server_queue;
host_id_t client_id;
queue_id_t other_queue = -1;

define_handler(INIT_REPLY)
{
    printf("[Client] Connected to server\n");
    client_id = msg->data.init_reply.id;
    return 0;
}

define_handler(STOP)
{
    printf("[Client] Server stopped\n");
    exit(0);
    return OK;
}

define_handler(CONNECT_REPLY)
{
    other_queue = msg->data.connect_reply.queue_id;
    printf("[Client] Succesfully connected with target client through queue: %i\n", other_queue);
    return OK;
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

void receive(union sigval sv)
{
    (void) sv;
    message msg;

    if(peek_message(client_queue, msg))
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

void run_receive()
{
    timer_t timer;
    struct sigevent event;
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = receive;
    event.sigev_notify_attributes = NULL;
    event.sigev_value.sival_ptr = NULL;
    timer_create(CLOCK_REALTIME, &event, &timer);
    struct timespec ms10 = {0, 10000000};
    struct itimerspec timer_value = {ms10, ms10};
    timer_settime(timer, 0, &timer_value, NULL);
}

void onexit()
{
    msgctl(client_queue, IPC_RMID, NULL);
    send_message(STOP, {client_id}, server_queue);
}

int main()
{
    key_t server_key = ftok(SERVER_KEY_PATHNAME, PROJECT_ID);
    if(server_key == -1)
    {
        printf("Failed to generate server queue key");
        return 0;
    }

    server_queue = msgget(server_key, QUEUE_PERMISSIONS);
    if(server_queue == -1)
    {
        printf("Failed to get server queue");
        return 0;
    }

    client_queue = msgget(IPC_PRIVATE, QUEUE_PERMISSIONS);
    if(server_queue == -1)
    {
        printf("Failed to get client queue");
        return 0;
    }

    send_message(INIT, {client_queue}, server_queue);
    message msg;
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

    run_receive();

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
                msgsnd(server_queue, &msg, sizeof(msg) - sizeof(long), 0);
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
                msgsnd(other_queue, &msg, sizeof(msg) - sizeof(long), 0);
            } else{
                printf("[Client] You are not connected to anyone\n");
            }
        }
    }
}