#include <common/common.h>

queue_id_t server_queue_id = SERVER_KEY;
queue_id_t client_queue_id;
queue_t client_queue;
queue_t server_queue;
host_id_t client_id;
queue_t other_queue;
bool closed = false;

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

define_handler(DISCONNECT)
{
    other_queue = 0;
    printf("[Client] Other client disconnected\n");
    return OK;
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
    mq_send(server_queue, (char*)&msg, sizeof(msg), INIT);
    if(receive_message(client_queue, msg))
    {
        if(msg.mtype == ERROR)
        {
            printf("[Client] Server is full\n");
            return 0;
        } else{
            printf("[Client] Connected to server\n");
            client_id = msg.data.init_reply.id;
        }
    }

    atexit(onexit);
    signal(SIGINT, exit);

    send_message(LIST_REQUEST, {client_id}, server_queue);
    if(receive_message(client_queue, msg)) {
        printf("[Client] Available clients:\n");
        for(size_t i=0;i<msg.data.list_reply.size; i++)
        {
            printf("Client %i: %s\n",
                    msg.data.list_reply.clients[i].id,
                    msg.data.list_reply.clients[i].available ? "available" : "not available");
        }
    }

    int do_cc_dc_tests;
    printf("Would you like to test connect/disconnect?");
    scanf("%i", &do_cc_dc_tests);

    if(do_cc_dc_tests)
    {
        printf("Provide target host id:");
        host_id_t target_id;
        scanf("%i", &target_id);

        message msg = {CONNECT_REQUEST, {}};
        msg.data.connect_request.id = client_id;
        msg.data.connect_request.id_to = target_id;
        mq_send(server_queue, (char*)&msg, sizeof(msg), 0);

        if(receive_message(client_queue, msg)) {
            if(msg.mtype == ERROR)
            {
                switch(msg.data.error.ec)
                {
                    case ALREADY_CONNECTED:
                        printf("[Client] Target is already connected\n");
                        break;
                    case INVALID_CLIENT_ID:
                        printf("[Client] Invalid target id\n");
                        break;
                }

                return 0;
            } else{
                printf("[Client] Succesfully connected with client\n");
                printf("[Client] Target client queue key: %s\n", msg.data.connect_reply.queue_id.code);
            }
        }

        // now we can safely talk with the client with target id
        // through his queue

        send_message(DISCONNECT, {client_id}, server_queue);
    }

    while(true)
    {
        if(receive_message(client_queue, msg))
        {
            error_code error;
            switch(msg.mtype)
            {
                handler_case_call(STOP, msg, error)
                handler_case_call(CONNECT_REPLY, msg, error)
                handler_case_call(DISCONNECT, msg, error)
            }
        }
    }
}