#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

int main(int argc, char** argv)
{
    if(argc != 4)
    {
        printf("zad3producer FIFO_path file_path read_block_size\n");
        return 1;
    }

    FILE* fifo_out = fopen(argv[1], "w");
    FILE* input_file = fopen(argv[2], "r");
    int read_block_size = atoi(argv[3]);

    if(!fifo_out || !input_file)
    {
        return 1;
    }

    char buffer[read_block_size + 1];
    char buffer_to_send[read_block_size + 24];

    while(fgets(buffer, read_block_size + 1, input_file) != NULL)
    {
        int length = snprintf(buffer_to_send,read_block_size + 24,"#%d#%s\n", getpid(), buffer);
        fwrite(buffer_to_send, sizeof(char), length, fifo_out);
        sleep(1);
    }

    fclose(fifo_out);
    fclose(input_file);

    return 0;
}