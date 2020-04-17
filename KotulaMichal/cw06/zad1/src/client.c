#include <common/common.h>

queue_id_t client_queue;
queue_id_t server_queue;
host_id_t client_id;
queue_id_t other_queue;

define_handler(STOP)
{
    printf("[Client] Server stopped\n");
    exit(0);
    return OK;
}

define_handler(CONNECT_REPLY)
{
    other_queue = msg->data.connect_reply.queue_id;
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
        msgsnd(server_queue, &msg, sizeof(msg), 0);

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
                printf("[Client] Target client queue key: %i\n", msg.data.connect_reply.queue_id);
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