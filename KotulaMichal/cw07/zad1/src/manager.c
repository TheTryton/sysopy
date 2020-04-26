#include <common/common.h>

#include <signal.h>
#include <unistd.h>
#include <wait.h>

int shared_memory_id;

void spawn(const char* executable, size_t n)
{
    for(size_t i = 0; i< n; i++)
    {
        if(fork() == 0 && execlp(executable, executable, NULL) == -1) exit(0);
    }
}

void at_exit() {
    for(size_t i=0;i<3;i++)
    {
        ring_buffer_remove(shared_memory->queues + i);
    }
    shmctl(shared_memory_id, IPC_RMID, NULL);
}

int main() {
    atexit(at_exit);
    signal(SIGINT, exit);

    key_t key = ftok(KEY_PATHNAME, PROJECT_ID);
    shared_memory_id = shmget(key, sizeof(struct shared), IPC_CREAT | IPC_EXCL | PERMISSIONS);

    if(shared_memory_id == -1)
    {
        printf("Failed to get shared memory id.\n");
        return 0;
    }

    shared_memory = shmat(shared_memory_id, NULL, 0);

    if(shared_memory == NULL)
    {
        printf("Failed to get shared memory id.\n");
        return 0;
    }

    int semaphore_id = semget(key, 3, IPC_CREAT | IPC_EXCL | PERMISSIONS);

    if(semaphore_id == -1)
    {
        printf("Failed to create semaphore.\n");
        return 0;
    }

    semaphore ring_buffer_semaphores[3] = {{semaphore_id, 0}, {semaphore_id, 1}, {semaphore_id, 2}};

    for(size_t i=0;i<3;i++)
    {
        shared_memory->queues[i] = ring_buffer_create(ring_buffer_semaphores[i]);
    }
    ring_buffer_fill(&shared_memory->queues[pending]);

    spawn("./zad1receiver", 2);
    spawn("./zad1packer", 2);
    spawn("./zad1sender", 2);

    while(wait(NULL) != -1);

    return 0;
}