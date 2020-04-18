#include <common/common.h>

queue_id_t server_queue_id = SERVER_KEY;
mqd_t server_queue;
typedef struct client {queue_t queue; queue_id_t qid; host_id_t id; struct client* other;} client;

client clients[MAX_CLIENTS_COUNT];

client* add_client(queue_id_t client_queue)
{
    static host_id_t id = 1;
    client* c = NULL;
    for(size_t i=0;i<MAX_CLIENTS_COUNT;i++)
    {
        if(clients[i].id == 0)
        {
            c = clients + i;
            c->queue = mq_open(client_queue.code, O_WRONLY);
            c->qid = client_queue;
            c->other = NULL;
            c->id = id++;
            break;
        }
    }
    return c;
}

error_code remove_client(host_id_t client_id)
{
    for(size_t i=0;i<MAX_CLIENTS_COUNT;i++)
    {
        if(clients[i].id == client_id)
        {
            send_message(STOP_REPLY, {0}, clients[i].queue)
            mq_close(clients[i].queue);
            clients[i].id=0;
            return OK;
        }
    }
    return INVALID_CLIENT_ID;
}

error_code disconnect(host_id_t client_id)
{
    for(size_t i=0;i<MAX_CLIENTS_COUNT;i++) {
        if (clients[i].id == client_id) {
            if(clients[i].other)
            {
                send_message(DISCONNECT, {client_id}, clients[i].other->queue);
                clients[i].other->other = NULL;
                clients[i].other = NULL;
            }
            return OK;
        }
    }
    return INVALID_CLIENT_ID;
}

error_code connect(host_id_t client_id, host_id_t target_id)
{
    client* c1 = NULL;
    client* c2 = NULL;
    for(size_t i=0;i<MAX_CLIENTS_COUNT;i++) {
        if (clients[i].id == client_id) {
            c1 = clients + i;
        }
        else if (clients[i].id == target_id) {
            c2 = clients + i;
        }
    }

    if(!c1 || !c2)
    {
        if(c1)
        {
            send_message(ERROR, { INVALID_CLIENT_ID }, c1->queue);
        }
        return INVALID_CLIENT_ID;
    }

    if(c1->other || c2->other)
    {
        send_message(ERROR, { ALREADY_CONNECTED }, c1->queue);
        return ALREADY_CONNECTED;
    }

    message msg = {CONNECT_REPLY};
    msg.data.connect_reply.queue_id = c1->qid;
    mq_send(c2->queue, (char*)&msg, sizeof(msg), CONNECT_REPLY);
    msg.data.connect_reply.queue_id = c2->qid;
    mq_send(c1->queue, (char*)&msg, sizeof(msg), CONNECT_REPLY);

    c1->other = c2;
    c2->other = c1;

    return OK;
}

define_handler(INIT)
{
    printf("[Server] INIT received\n");
    client* c = add_client(msg->data.init.queue_id);
    if(c)
    {
        send_message(INIT_REPLY, { c->id }, c->queue);
        return OK;
    } else{
        queue_id_t qid = msg->data.init.queue_id;
        mqd_t q = mq_open(qid.code, O_WRONLY);
        send_message(ERROR, { SERVER_FULL }, q);
        mq_close(q);
        return SERVER_FULL;
    }
}

define_handler(STOP)
{
    printf("[Server] STOP received\n");
    error_code ec;
    if((ec = disconnect(msg->data.stop.id)) != OK)
    {
        return ec;
    }
    if((ec = remove_client(msg->data.stop.id)) != OK)
    {
        return ec;
    }
    return OK;
}

define_handler(DISCONNECT)
{
    printf("[Server] DISCONNECT received\n");
    error_code ec;
    if((ec = disconnect(msg->data.disconnect.id)) != OK)
    {
        return ec;
    }
    return OK;
}

define_handler(LIST_REQUEST)
{
    printf("[Server] LIST_REQUEST received\n");
    message msg_res = {LIST_REPLY};
    int size = 0;
    for(size_t i=0;i<MAX_CLIENTS_COUNT;i++)
    {
        if(clients[i].id != 0 && clients[i].id != msg->data.list_request.id)
        {
            msg_res.data.list_reply.clients[size].id = clients[i].id;
            msg_res.data.list_reply.clients[size].available = !clients[i].other;
            size++;
        }
    }
    msg_res.data.list_reply.size = size;
    mqd_t qid;
    for(size_t i=0;i<MAX_CLIENTS_COUNT;i++)
    {
        if(clients[i].id == msg->data.list_request.id)
        {
            qid = clients[i].queue;
        }
    }
    mq_send(qid, (char*)&msg_res, sizeof(msg_res), 0);
    return OK;
}

define_handler(CONNECT_REQUEST)
{
    printf("[Server] CONNECT received\n");

    return connect(msg->data.connect_request.id, msg->data.connect_request.id_to);
}

void onexit()
{
    for(int i=0;i<MAX_CLIENTS_COUNT;i++)
    {
        if(clients[i].queue)
        {
            send_message(STOP, { 0 }, clients[i].queue);
            message msg;
            while(receive_message(clients[i].queue, msg))
            {
                if(msg.mtype == STOP_REPLY)
                {
                    printf("[Server] Client %i disconnected\n", clients[i].id);
                    break;
                }
            }
        }
    }

    mq_close(server_queue);
    mq_unlink(server_queue_id.code);
}

int main()
{
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(message);
    attr.mq_curmsgs = 0;

    server_queue = mq_open(server_queue_id.code, O_CREAT | O_EXCL | O_RDONLY, QUEUE_PERMISSIONS, &attr);
    if(server_queue == -1)
    {
        printf("Failed to get server queue");
        return 0;
    }

    mq_getattr(server_queue, &attr);

    atexit(onexit);
    signal(SIGINT, exit);

    while(true)
    {
        message msg;
        if(receive_message(server_queue, msg))
        {
            error_code error;
            switch(msg.mtype)
            {
                handler_case_call(INIT, msg, error)
                handler_case_call(STOP, msg, error)
                handler_case_call(LIST_REQUEST, msg, error)
                handler_case_call(CONNECT_REQUEST, msg, error)
                handler_case_call(DISCONNECT, msg, error)
            }

            if(error)
            {
                switch (error) {
                    case ALREADY_CONNECTED:
                        printf("[Server] Client is already connected\n");
                        break;
                    case SERVER_FULL:
                        printf("[Server] Server is full\n");
                        break;
                    case INVALID_CLIENT_ID:
                        printf("[Server] Invalid client id\n");
                        break;
                }
            }
        }
    }
}