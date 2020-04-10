#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv)
{
    if(argc != 4)
    {
        printf("zad3consumer FIFO_path file_path read_block_size\n");
        return 1;
    }

    FILE* fifo_in = fopen(argv[1], "r");
    FILE* output_file = fopen(argv[2], "w+");
    int read_block_size = atoi(argv[3]);

    if(!fifo_in || !output_file)
    {
        return 1;
    }

    char buffer[read_block_size + 1];
    int pid;
    while(fscanf(fifo_in, "#%d#%s\n", &pid, buffer) != EOF){
        fprintf(output_file, "%s\n", buffer);
    }

    fclose(fifo_in);
    fclose(output_file);

    return 0;
}