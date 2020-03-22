#include <common.h>
#include <time.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/resource.h>

clock_t start;
int multiplications_done = 0;
double max_seconds;

bool multiply_fragment(matrix a, matrix b, matrix* c, int column_start_b, int column_end_b)
{
    bool any = false;
    for(int row = 0; row < a.size.rows; row++)
    {
        for(int column = column_start_b; column < column_end_b && column < b.size.columns; column++)
        {
            if(clock() - start > max_seconds*CLOCKS_PER_SEC)
            {
                exit(multiplications_done);
            }

            any=true;
            c->m[row][column] = 0;
            for(int k = 0; k < a.size.columns; k++)
            {
                c->m[row][column] += a.m[row][k] * b.m[k][column];
            }
        }
    }
    return any;
}

void save_fragment(const char* file_name, matrix c, int column_start_b, int column_end_b, bool shared, int index, int worker_index)
{
    if(shared)
    {
        int file_handle = open(file_name, O_WRONLY);

        if(file_handle == -1)
        {
            file_handle = open(file_name, O_WRONLY | O_CREAT, S_IRWXU);
        }

        if(flock(file_handle, LOCK_EX) == 0) {

            if(lseek(file_handle,0, SEEK_END) == 0)
            {
                prepare_file_for_matrix(file_handle, c.size);
            }

            lseek(file_handle,0, SEEK_SET);

            for (int row = 0; row < c.size.rows; row++) {
                for (int column = column_start_b; column < column_end_b && column < c.size.columns; column++) {
                    save_matrix_element(file_handle, c, row, column);
                }
            }

            flock(file_handle, LOCK_UN);
        }

        close(file_handle);
    } else{
        int size = snprintf(NULL, 0, "fragment%i%i", index, worker_index) + 2;
        char* filename = calloc(size, sizeof(char));
        snprintf(filename, size, "fragment%i%i", index, worker_index);

        int file_handle = open(filename, O_WRONLY | O_CREAT, S_IRWXU);

        free(filename);

        if(file_handle == -1)
        {
            return;
        }

        for (int row = 0; row < c.size.rows; row++) {
            for (int column = column_start_b; column < column_end_b && column < c.size.columns; column++) {
                save_element(file_handle, c.m[row][column]);
                save_separator(file_handle, '\t');
            }
            save_separator(file_handle, '\n');
        }

        close(file_handle);
    }
}

int main(int argc, char** argv)
{
    if(argc != 9)
    {
        return 0;
    }

    start = clock();

    int fh_ts = open(argv[1], O_RDONLY);
    multiplication_triples triples = load_multiplication_triples(fh_ts);
    close(fh_ts);

    int column_start_b = atoi(argv[2]);
    int column_end_b = atoi(argv[3]);
    max_seconds = atof(argv[4]);
    bool shared = (strcmp(argv[5], "shared") == 0) ? true : false;
    int worker_index = atoi(argv[6]);
    float max_time = atof(argv[7]);
    float max_vmemory = atof(argv[8]);

    struct rlimit resource_limit;
    resource_limit.rlim_cur = max_vmemory*1024*1024;
    resource_limit.rlim_max = max_vmemory*1024*1024;
    setrlimit(RLIMIT_AS, &resource_limit);
    resource_limit.rlim_cur = max_time;
    resource_limit.rlim_max = max_time;
    setrlimit(RLIMIT_CPU, &resource_limit);

    for(int i=0;i<triples.size;i++)
    {
        if(clock() - start > max_seconds*CLOCKS_PER_SEC)
        {
            break;
        }

        int fh_a = open(triples.triples[i].a, O_RDONLY);
        int fh_b = open(triples.triples[i].b, O_RDONLY);

        if(fh_a == -1 || fh_b == -1)
        {
            return -1;
        }

        matrix a = load_matrix(fh_a);
        matrix b = load_matrix(fh_b);

        close(fh_a);
        close(fh_b);

        matrix c = create_matrix(a.size.rows, b.size.columns);

        if(multiply_fragment(a, b, &c, column_start_b, column_end_b) || !shared)
        {
            save_fragment(triples.triples[i].c, c, column_start_b, column_end_b, shared, i, worker_index);
            multiplications_done++;
        }

        free_matrix(&a);
        free_matrix(&b);
        free_matrix(&c);
    }

    free_multiplication_triples(&triples);

    return multiplications_done;
}