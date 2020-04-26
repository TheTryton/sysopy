#include <common/common.h>

#include <wait.h>

int shared_memory_id;

void spawn(const char* executable, size_t n)
{
    for(size_t i = 0; i < n; i++)
    {
        if(fork() == 0 && execlp(executable, executable, NULL) == -1) exit(0);
    }
}

void at_exit() {
    signal(SIGINT, SIG_IGN);
    kill(0, SIGINT);

    sem_close(rb_semaphores[0].sem);
    sem_close(rb_semaphores[1].sem);
    sem_close(rb_semaphores[2].sem);
    sem_unlink(SEM1);
    sem_unlink(SEM2);
    sem_unlink(SEM3);
    munmap(shared_memory, sizeof(struct shared));
    shm_unlink(SHARED_MEMORY);
}

int main() {
    atexit(at_exit);
    signal(SIGINT, exit);

    shared_memory_id = shm_open(SHARED_MEMORY, O_CREAT | O_RDWR, PERMISSIONS);
    ftruncate(shared_memory_id, sizeof(struct shared));

    if(shared_memory_id == -1)
    {
        printf("Failed to get shared memory id.\n");
        return 0;
    }

    shared_memory = mmap(NULL, sizeof(struct shared), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_id, 0);

    if(shared_memory == NULL)
    {
        printf("Failed to get shared memory id.\n");
        return 0;
    }

    rb_semaphores[0].sem = sem_open(SEM1, O_CREAT, PERMISSIONS, 1);
    rb_semaphores[1].sem = sem_open(SEM2, O_CREAT, PERMISSIONS, 1);
    rb_semaphores[2].sem = sem_open(SEM3, O_CREAT, PERMISSIONS, 1);

    ring_buffer empty = {0,0,0};

    for(size_t i=0;i<3;i++) {
        if(rb_semaphores[i].sem == SEM_FAILED)
        {
            printf("Failed to create semaphore.\n");
            return 0;
        }
        shared_memory->queues[i] = empty;
    }

    ring_buffer_fill(rb_semaphores[pending], &shared_memory->queues[pending]);

    spawn("./zad2receiver", 2);
    spawn("./zad2packer", 2);
    spawn("./zad2sender", 2);

    while(wait(NULL) != -1);

    return 0;
}