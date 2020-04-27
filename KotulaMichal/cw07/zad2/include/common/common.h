#pragma once

#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
#include <unistd.h>

#include <sys/time.h>
#include <time.h>

#define SHARED_MEMORY "/shared"
#define SEM1 "/sem1"
#define SEM2 "/sem2"
#define SEM3 "/sem3"
#define PERMISSIONS 0600
#define MAX_PACKAGES 100

typedef enum package_status
{
    pending = 0,
    received = 1,
    packed = 2,
    sent = pending
} package_status;

typedef struct package
{
    size_t size;
} package;

typedef struct semaphore
{
    sem_t* sem;
} semaphore;

typedef struct ring_buffer
{
    size_t push_index;
    size_t pop_index;
    size_t count_elements;
    package packages[MAX_PACKAGES];
} ring_buffer;

int lock(semaphore semaphore)
{
    return sem_wait(semaphore.sem);
}

int unlock(semaphore semaphore)
{
    return sem_post(semaphore.sem);
}

bool ring_buffer_put(semaphore semaphore, ring_buffer* buff, package pkg)
{
    lock(semaphore);
    if(buff->count_elements != MAX_PACKAGES)
    {
        buff->count_elements++;
        buff->packages[buff->push_index]=pkg;
        buff->push_index = (buff->push_index + 1) % MAX_PACKAGES;
        unlock(semaphore);
        return true;
    } else{
        unlock(semaphore);
        return false;
    }
}

bool ring_buffer_pop(semaphore semaphore, ring_buffer* buff, package* pkg)
{
    lock(semaphore);
    if(buff->count_elements != 0)
    {
        buff->count_elements--;
        *pkg = buff->packages[buff->pop_index];
        buff->pop_index = (buff->pop_index + 1) % MAX_PACKAGES;
        unlock(semaphore);
        return true;
    } else{
        unlock(semaphore);
        return false;
    }
}

void ring_buffer_fill(semaphore semaphore, ring_buffer* buff) {
    lock(semaphore);

    buff->count_elements = MAX_PACKAGES;
    buff->push_index = buff->pop_index;

    unlock(semaphore);
}

size_t ring_buffer_size(semaphore semaphore, ring_buffer* buff)
{
    lock(semaphore);
    size_t size = buff->count_elements;
    unlock(semaphore);
    return size;
}

struct shared
{
    ring_buffer queues[3];
} *shared_memory;

semaphore rb_semaphores[3];

bool get(struct shared* shared_memory, package_status pkg_status, package* pkg)
{
    return ring_buffer_pop(rb_semaphores[pkg_status],shared_memory->queues + pkg_status, pkg);
}

bool put(struct shared* shared_memory, package_status pkg_status, package pkg)
{
    return ring_buffer_put(rb_semaphores[pkg_status],shared_memory->queues + pkg_status, pkg);
}

void worker_exit() {
    sem_close(rb_semaphores[0].sem);
    sem_close(rb_semaphores[1].sem);
    sem_close(rb_semaphores[2].sem);
    munmap(shared_memory, sizeof(struct shared));
}

int worker(int (*runner)())
{
    atexit(worker_exit);
    signal(SIGINT, exit);

    int shared_memory_id = shm_open(SHARED_MEMORY, O_RDWR, PERMISSIONS);

    if(shared_memory_id == -1)
    {
        printf("Failed to get shared memory id.\n");
        return 0;
    }

    shared_memory = mmap(NULL, sizeof(struct shared), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_id, 0);

    if(shared_memory == NULL)
    {
        printf("Failed to get shared memory.\n");
        return 0;
    }

    rb_semaphores[0].sem = sem_open(SEM1, 0);
    rb_semaphores[1].sem = sem_open(SEM2, 0);
    rb_semaphores[2].sem = sem_open(SEM3, 0);

    for(size_t i=0;i<3;i++) {
        if(rb_semaphores[i].sem == SEM_FAILED)
        {
            printf("Failed to create semaphore.\n");
            return 0;
        }
    }

    return runner();
}

#define log(x, args...)\
  {\
    struct timeval time_now;\
    gettimeofday(&time_now, NULL);\
    struct tm* time_str_tm = gmtime(&time_now.tv_sec);\
    time_t timer = time(NULL);\
    printf("(%d %02i:%02i:%02i:%06li)" x "\n",\
    getpid(), time_str_tm->tm_hour, time_str_tm->tm_min, time_str_tm->tm_sec, time_now.tv_usec,\
    args);\
  }


