#include <common/common.h>

int receiver()
{
    while(true)
    {
        package pkg;
        if(!get(shared_memory, packed, &pkg))
        {
            printf("[Receiver] No packed packages.\n");
            continue;
        }

        pkg.size *= 3;

        while(!put(shared_memory, sent, pkg))
        {
            printf("[Receiver] Pending package queue is full.\n");
        }

        printf("Wyslalem zamowienie o wielkosci: %lu. Liczba zamowien do przygotowania: %lu. Liczba zamowien do wyslania %lu,\n",
               pkg.size,
               ring_buffer_size(rb_semaphores[received], &shared_memory->queues[received]),
               ring_buffer_size(rb_semaphores[received],&shared_memory->queues[packed]));

        struct timespec ms100 = {0,100000000};
        nanosleep(&ms100, &ms100);
    }
}

int main() {
    return worker(receiver);
}