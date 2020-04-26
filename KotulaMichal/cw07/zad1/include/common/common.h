#pragma once

#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#define KEY_PATHNAME "/home/michal/"
#define PROJECT_ID 'S'
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
    int semaphore_id;
    int semaphore_num;
} semaphore;

typedef struct ring_buffer
{
    semaphore semaphore;
    size_t push_index;
    size_t pop_index;
    size_t count_elements;
    package packages[MAX_PACKAGES];
} ring_buffer;

int lock(semaphore semaphore)
{
    struct sembuf wait_increment[2] = {{semaphore.semaphore_num, 0, SEM_UNDO}, {semaphore.semaphore_num, 1, SEM_UNDO}};
    return semop(semaphore.semaphore_id, wait_increment, 2);
}

int unlock(semaphore semaphore)
{
    struct sembuf semaphor_decrement = {semaphore.semaphore_num, -1, SEM_UNDO};
    return semop(semaphore.semaphore_id, &semaphor_decrement, 1);
}

ring_buffer ring_buffer_create(semaphore semaphore)
{
    ring_buffer buffer = {semaphore, 0,0,0};
    return buffer;
}

void ring_buffer_remove(ring_buffer* buff)
{
    semctl(buff->semaphore.semaphore_id, buff->semaphore.semaphore_num, IPC_RMID, NULL);
}

bool ring_buffer_put(ring_buffer* buff, package pkg)
{
    lock(buff->semaphore);
    if(buff->count_elements != MAX_PACKAGES)
    {
        buff->count_elements++;
        buff->packages[buff->push_index]=pkg;
        buff->push_index = (buff->push_index + 1) % MAX_PACKAGES;
        unlock(buff->semaphore);
        return true;
    } else{
        unlock(buff->semaphore);
        return false;
    }
}

bool ring_buffer_pop(ring_buffer* buff, package* pkg)
{
    lock(buff->semaphore);
    if(buff->count_elements != 0)
    {
        buff->count_elements--;
        *pkg = buff->packages[buff->pop_index];
        buff->pop_index = (buff->pop_index + 1) % MAX_PACKAGES;
        unlock(buff->semaphore);
        return true;
    } else{
        unlock(buff->semaphore);
        return false;
    }
}

void ring_buffer_fill(ring_buffer* buff) {
    lock(buff->semaphore);

    buff->count_elements = MAX_PACKAGES;
    buff->push_index = buff->pop_index;

    unlock(buff->semaphore);
}

size_t ring_buffer_size(ring_buffer* buff)
{
    lock(buff->semaphore);
    size_t size = buff->count_elements;
    unlock(buff->semaphore);
    return size;
}

struct shared
{
    ring_buffer queues[3];
} *shared_memory;

bool get(struct shared* shared_memory, package_status pkg_status, package* pkg)
{
    return ring_buffer_pop(shared_memory->queues + pkg_status, pkg);
}

bool put(struct shared* shared_memory, package_status pkg_status, package pkg)
{
    return ring_buffer_put(shared_memory->queues + pkg_status, pkg);
}

int worker(int (*runner)())
{
    key_t key = ftok(KEY_PATHNAME, PROJECT_ID);
    int shared_memory_id = shmget(key, 0, PERMISSIONS);

    if(shared_memory_id == -1)
    {
        printf("Failed to get shared memory id.\n");
        return 0;
    }

    shared_memory = shmat(shared_memory_id, NULL, 0);

    if(shared_memory == NULL)
    {
        printf("Failed to get shared memory.\n");
        return 0;
    }

    return runner();
}


