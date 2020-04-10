#include <stdio.h>

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("zad2 file_path\n");
        return 1;
    }

    FILE* input_file = fopen(argv[1], "r");
    FILE* sort = popen("sort", "w");

    if(!input_file || !sort)
    {
        return 1;
    }

    int c;
    while((c = fgetc(input_file)) != EOF)
    {
        fputc(c, sort);
    }

    fclose(input_file);
    pclose(sort);

    return 0;
}