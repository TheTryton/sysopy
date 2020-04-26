#include <common/common.h>

int receiver()
{
    while(true)
    {
        package pkg;
        if(!get(shared_memory, pending, &pkg))
        {
            printf("[Receiver] No pending packages.\n");
            continue;
        }

        pkg.size = rand() % 10 + 1;

        while(!put(shared_memory, received, pkg))
        {
            printf("[Receiver] Received package queue is full.\n");
        }

        printf("Dodalem liczbe: %lu. Liczba zamowien do przygotowania: %lu. Liczba zamowien do wyslania %lu.\n",
               pkg.size, ring_buffer_size(&shared_memory->queues[received]), ring_buffer_size(&shared_memory->queues[packed]));

        struct timespec ms100 = {0,100000000};
        nanosleep(&ms100, &ms100);
    }
}

int main() {
    return worker(receiver);
}