#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <pthread.h>
#include <time.h>
#include <sys/errno.h>

typedef enum error_code {
    none = 0,
    deadlock = EDEADLK,
    invalid_value = EINVAL,
    no_thread = ESRCH,
    insufficent_resources = EAGAIN,
    no_permission = EPERM
} error_code;

const char* translate_error(error_code e)
{
    switch(e)
    {
        case none:
            return "No error.\n";
        case deadlock:
            return "Deadlock detected.\n";
        case invalid_value:
            return "Thread is not joinable/another thread is waiting to join with this thread.\n";
        case no_thread:
            return "Invalid thread ID.\n";
        case insufficent_resources:
            return "Insufficent resources to create new thread.\n";
        case no_permission:
            return "No permission to set scheduling policy and parameters attributes when creating thread.\n";
        default:
            return "Invalid error code.\n";
    }
}

typedef struct mutex
{
    pthread_mutex_t m;
} mutex;

error_code mcreate(mutex* m)
{
    return pthread_mutex_init(&m->m, NULL);
}

error_code mdestroy(mutex* m)
{
    return pthread_mutex_destroy(&m->m);
}

error_code mlock(mutex* m)
{
    return pthread_mutex_lock(&m->m);
}

error_code mtrylock(mutex* m)
{
    return pthread_mutex_trylock(&m->m);
}

error_code munlock(mutex* m)
{
    return pthread_mutex_unlock(&m->m);
}

typedef struct condition_variable
{
    pthread_cond_t cv;
} condition_variable;

error_code cvcreate(condition_variable* cv)
{
    return pthread_cond_init(&cv->cv, NULL);
}

error_code cvdestroy(condition_variable* cv)
{
    return pthread_cond_destroy(&cv->cv);
}

error_code cvwait(condition_variable* cv, mutex* m, bool(*condition)(void**, size_t), void** args, size_t argc)
{
    while(!condition(args, argc))
    {
        error_code error = pthread_cond_wait(&cv->cv, &m->m);
        if(error != none)
        {
            return error;
        }
    }
    return none;
}

error_code cvnotify_one(condition_variable* cv)
{
    return pthread_cond_signal(&cv->cv);
}

error_code cvnotify_all(condition_variable* cv)
{
    return pthread_cond_broadcast(&cv->cv);
}

typedef struct thread
{
    pthread_t id;
} thread;

typedef enum join_state
{
    exited,
    canceled,
    error
} join_state;

typedef union join_result
{
    int exit_code;
    error_code error_code;
} join_result;

typedef struct init_main
{
    int(*main)(void**, size_t);
    void** args;
    size_t argc;
} init_main;

void* tthread_main(void* arg)
{
    int* result = calloc(1, sizeof(int));
    init_main init = *(init_main*)arg;
    free(arg);
    *result = init.main(init.args, init.argc);
    return result;
}

error_code tcreate(thread* t, int(*thread_main)(void**, size_t), void** args, size_t argc)
{
    init_main* arg = (init_main*)calloc(1, sizeof(init_main));
    arg->main = thread_main;
    arg->args = args;
    arg->argc = argc;
    return pthread_create(&t->id, NULL, tthread_main, arg);
}

join_state tjoin(thread* t, join_result* result)
{
    void* join_result;

    error_code err;
    if((err = pthread_join(t->id, &join_result)) != 0)
    {
        if(result)
        {
            result->error_code = err;
        }
        return error;
    } else{
        if(join_result == PTHREAD_CANCELED)
        {
            return canceled;
        } else{
            if(result)
            {
                result->exit_code = *(int*)join_result;
            }
            free(join_result);
            return exited;
        }
    }
}

thread tthis_thread()
{
    thread tt = {pthread_self()};
    return tt;
}

void texit(int* value)
{
    pthread_exit(value);
}

typedef enum stop_reaction
{
    instant = PTHREAD_CANCEL_ASYNCHRONOUS,
    deferred = PTHREAD_CANCEL_DEFERRED
} stop_reaction;

error_code tstop(thread* t)
{
    return pthread_cancel(t->id);
}

void tset_stop_reaction(stop_reaction reaction)
{
    int dummy;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &dummy);
    pthread_setcanceltype(reaction, &dummy);
}

typedef size_t client_id;

typedef struct waitable_queue
{
    client_id* q;
    size_t size;
    size_t capacity;
    size_t push_index;
    size_t pop_index;
    mutex m;
    condition_variable cv;
} waitable_queue;

error_code wq_create(waitable_queue* wq, size_t capacity)
{
    wq->q = (client_id*)calloc(capacity, sizeof(client_id));
    wq->size = 0;
    wq->capacity = capacity;
    wq->push_index = 0;
    wq->pop_index = 0;

    error_code error = mcreate(&wq->m);

    if(error != none)
    {
        free(wq->q);

        return error;
    }

    error = cvcreate(&wq->cv);

    if(error != none)
    {
        free(wq->q);
        mdestroy(&wq->m);

        return error;
    }

    return error;
}

void wq_destroy(waitable_queue* wq)
{
    free(wq->q);
    mdestroy(&wq->m);
    cvdestroy(&wq->cv);
}

bool wq_available(void** args, size_t argc)
{
    return ((waitable_queue*)args[0])->size > 0;
}

client_id wq_get(waitable_queue* wq, bool dolock)
{
    if(dolock)
    {
        mlock(&wq->m);
    }
    void* args[1] = {wq};
    cvwait(&wq->cv, &wq->m, wq_available, args, 1);
    client_id id = wq->q[wq->pop_index++];
    wq->pop_index = wq->pop_index % wq->capacity;
    wq->size--;
    cvnotify_all(&wq->cv);
    if(dolock)
    {
        munlock(&wq->m);
    }
    return id;
}

bool wq_tryget(waitable_queue* wq, client_id* front, bool dolock)
{
    if(dolock)
    {
        mlock(&wq->m);
    }
    if(wq->size > 0)
    {
        *front = wq->q[wq->pop_index++];
        wq->pop_index = wq->pop_index % wq->capacity;
        wq->size--;
        if(dolock) {
            cvnotify_all(&wq->cv);
            munlock(&wq->m);
        }
        return true;
    } else {
        if(dolock)
        {
            munlock(&wq->m);
        }
        return false;
    }
}

bool wq_trypush(waitable_queue* wq, client_id id, bool dolock)
{
    if(dolock)
    {
        mlock(&wq->m);
    }
    if(wq->size != wq->capacity)
    {
        wq->q[wq->push_index++] = id;
        wq->push_index = wq->push_index % wq->capacity;
        wq->size++;
        if(dolock)
        {
            cvnotify_all(&wq->cv);
            munlock(&wq->m);
        }
        return true;
    } else {
        if(dolock)
        {
            munlock(&wq->m);
        }
        return false;
    }
}

waitable_queue barber_queue;
bool is_sleeping = false;
bool no_remaining_clients = false;

int barber_main(void** args, size_t argc)
{
    while(true)
    {
        client_id processed_client;
        size_t clients_waiting;

        mlock(&barber_queue.m);
        if(!wq_tryget(&barber_queue,&processed_client, false))
        {
            if(no_remaining_clients)
            {
                munlock(&barber_queue.m);
                break;
            }

            printf("Golibroda: ide spac.\n");
            is_sleeping=true;

            processed_client = wq_get(&barber_queue, false);

            is_sleeping=false;

            clients_waiting = barber_queue.size;

            munlock(&barber_queue.m);
        } else
        {
            clients_waiting = barber_queue.size;
            munlock(&barber_queue.m);
        }

        printf("Golibroda: czeka %lu klientow, gole klienta %lu.\n", clients_waiting, processed_client);

        size_t msec_sleep = rand() % 2000 + 1000;
        struct timespec ts;
        ts.tv_nsec = msec_sleep%1000*1000000;
        ts.tv_sec = msec_sleep/1000;
        nanosleep(&ts, &ts);
    }

    return 0;
}

int client_main(void** args, size_t argc) {
    client_id id = *((client_id *) args[0]);
    free(args[0]);

    mlock(&barber_queue.m);
    while (!wq_trypush(&barber_queue, id, false)) {
        munlock(&barber_queue.m);
        printf("Zajete; %lu.\n", id);
        size_t msec_sleep = rand() % 2000 + 1000;
        struct timespec ts;
        ts.tv_nsec = msec_sleep % 1000 * 1000000;
        ts.tv_sec = msec_sleep / 1000;
        nanosleep(&ts, &ts);
        mlock(&barber_queue.m);
    }

    size_t remaining_capacity = barber_queue.capacity - barber_queue.size;

    if (is_sleeping) {
        printf("Budze golibrode; %lu.\n", id);
    } else {
        printf("Poczekalnia, wolne miejsca: %lu; %lu.\n", remaining_capacity, id);
    }

    cvnotify_one(&barber_queue.cv);

    munlock(&barber_queue.m);

    return 0;
}

int main(int argc, char** argv) {
    if(argc != 3)
    {
        printf("zad1 chairs_count clients_count\n");
        return 0;
    }

    srand(time(NULL));

    size_t chairs_count = strtoll(argv[1], NULL, 10);
    size_t clients_count = strtoll(argv[2], NULL, 10);

    wq_create(&barber_queue, chairs_count);

    thread clients[clients_count];
    thread barber;

    tcreate(&barber,barber_main, NULL, 0);

    for(size_t i=0;i<clients_count;i++)
    {
        client_id* id = (client_id*)calloc(1, sizeof(client_id));
        *id = i;
        void* args[1] = {id};
        tcreate(&clients[i], client_main, args, 1);

        size_t msec_sleep = rand() % 1500 + 500;
        struct timespec ts;
        ts.tv_nsec = msec_sleep%1000*1000000;
        ts.tv_sec = msec_sleep/1000;
        nanosleep(&ts, &ts);
    }

    mlock(&barber_queue.m);
    no_remaining_clients = true;
    munlock(&barber_queue.m);

    for(size_t i=0;i<clients_count;i++) {
        tjoin(&clients[i], NULL);
    }

    tjoin(&barber, NULL);

    wq_destroy(&barber_queue);

    return 0;
}