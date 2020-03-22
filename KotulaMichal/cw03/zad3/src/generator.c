#include <common.h>

int main(int argc, char** argv)
{
    if(argc != 5)
    {
        return 0;
    }

    int count = atoi(argv[2]);
    int min = atoi(argv[3]);
    int max = atoi(argv[4]);

    int file_handle_lista = open(argv[1], O_WRONLY | O_CREAT, S_IRWXU);

    for(int i=0;i<count;i++)
    {
        int N = rand() %(max - min) + min;
        int M = rand() %(max - min) + min;
        int P = rand() %(max - min) + min;

        matrix a = create_matrix(N, M);
        for(int r = 0; r < N; r++)
        {
            for(int c = 0; c < M; c++)
            {
                a.m[r][c] = rand() % 10;
            }
        }
        matrix b = create_matrix(M, P);
        for(int r = 0; r < M; r++)
        {
            for(int c = 0; c < P; c++)
            {
                b.m[r][c] = rand() % 10 + 5;
            }
        }

        char filename[FILENAME_MAX];

        int fn_size = snprintf(filename, FILENAME_MAX, "a%i", i);
        int file_handle = open(filename, O_WRONLY | O_CREAT, S_IRWXU);
        save_matrix(file_handle, a);
        write(file_handle_lista, filename, fn_size);
        write(file_handle_lista, " ", 1);
        close(file_handle);

        fn_size = snprintf(filename, FILENAME_MAX, "b%i", i);
        file_handle = open(filename, O_WRONLY | O_CREAT, S_IRWXU);
        save_matrix(file_handle, b);
        write(file_handle_lista, filename, fn_size);
        write(file_handle_lista, " ", 1);
        close(file_handle);

        fn_size = snprintf(filename, FILENAME_MAX, "c%i", i);
        write(file_handle_lista, filename, fn_size);
        write(file_handle_lista, "\n", 1);

        free_matrix(&a);
        free_matrix(&b);
    }

    close(file_handle_lista);
}