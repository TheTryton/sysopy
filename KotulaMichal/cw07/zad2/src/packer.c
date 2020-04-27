#include <common/common.h>

int receiver()
{
    while(true)
    {
        struct timespec ms100 = {0,100000000};

        package pkg;
        if(!get(shared_memory, received, &pkg))
        {
            printf("[Receiver] No received packages.\n");
            nanosleep(&ms100, &ms100);
            continue;
        }

        pkg.size *= 2;

        while(!put(shared_memory, packed, pkg))
        {
            printf("[Receiver] Packed packages queue is full.\n");
            nanosleep(&ms100, &ms100);
        }

        log("Przygotowalem zamowienie o wielkosci: %lu. Liczba zamowien do przygotowania: %lu. Liczba zamowien do wyslania %lu,\n",
               pkg.size,
               ring_buffer_size(rb_semaphores[received], &shared_memory->queues[received]),
               ring_buffer_size(rb_semaphores[received],&shared_memory->queues[packed]));

        nanosleep(&ms100, &ms100);
    }
}

int main() {
    return worker(receiver);
}