#include <common/common.h>

int receiver()
{
    while(true)
    {
        struct timespec ms100 = {0,100000000};

        package pkg;
        if(!get(shared_memory, pending, &pkg))
        {
            printf("[Receiver] No pending packages.\n");
            nanosleep(&ms100, &ms100);
            continue;
        }

        pkg.size = rand() % 10 + 1;

        while(!put(shared_memory, received, pkg))
        {
            printf("[Receiver] Received package queue is full.\n");
            nanosleep(&ms100, &ms100);
        }

        log("Dodalem liczbe: %lu. Liczba zamowien do przygotowania: %lu. Liczba zamowien do wyslania %lu.\n",
               pkg.size, ring_buffer_size(&shared_memory->queues[received]), ring_buffer_size(&shared_memory->queues[packed]));

        nanosleep(&ms100, &ms100);
    }
}

int main() {
    return worker(receiver);
}