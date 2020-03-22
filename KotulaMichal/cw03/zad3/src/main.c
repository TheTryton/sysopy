#include <common.h>
#include <stdbool.h>
#include <wait.h>
#include <math.h>
#include <sys/resource.h>

int main(int argc, char** argv)
{
    if(argc != 7)
    {
        return 0;
    }

    bool shared = (strcmp(argv[3], "shared") == 0) ? true : false;

    int workers_count = atoi(argv[2]);
    pid_t workers[workers_count];

    int fh_mts = open(argv[1], O_RDONLY);
    multiplication_triples triples = load_multiplication_triples(fh_mts);
    close(fh_mts);

    int max_columns = 0;

    for(int i=0;i<triples.size;i++)
    {
        int fh_b = open(triples.triples[i].b, O_RDONLY);
        matrix_descriptor md = load_matrix_descriptor(fh_b);
        close(fh_b);
        max_columns = max_columns > md.columns ? max_columns : md.columns;
    }

    int columns_for_worker = ceil((float)max_columns / (float)workers_count);

    for(int i = 0; i < workers_count; i++)
    {
        if((workers[i] = fork()) == 0)
        {
            char columns_start_b[16];
            char columns_end_b[16];
            char worker_index[16];

            snprintf(columns_start_b, 16, "%i", i*columns_for_worker);
            snprintf(columns_end_b, 16, "%i", (i+1)*columns_for_worker);
            snprintf(worker_index, 16, "%i", i);

            execl("./child", "./child", argv[1], columns_start_b, columns_end_b, argv[3], argv[4], worker_index, argv[5], argv[6], NULL);
        }
    }

    int status;

    for(int i = 0; i < workers_count; i++) {
        waitpid(workers[i], &status, 0);
        printf("Proces %i wykonal %i mnozen macierzy\n", workers[i], WEXITSTATUS(status));
    }

    if(!shared)
    {
        for(int i=0;i<triples.size;i++)
        {
            pid_t child = fork();
            if(child == 0)
            {
                char** argv = calloc(workers_count+2, sizeof(char*));
                argv[workers_count] = NULL;
                argv[0] = "/usr/bin/paste";
                for(int w=0;w<workers_count;w++)
                {
                    int size = snprintf(NULL, 0, "fragment%i%i", i, w) + 2;
                    argv[w+1] = calloc(size, sizeof(char));
                    snprintf(argv[w+1], size, "fragment%i%i", i, w);
                    int fh = open(argv[w+1], O_RDONLY);

                    if(fh == -1)
                    {
                        return 0;
                    }

                    close(fh);
                }

                int fd = open(triples.triples[i].c, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

                dup2(fd, 1);
                dup2(fd, 2);

                close(fd);

                execv("/usr/bin/paste",argv);

                for(int w=0;w<workers_count;w++)
                {
                    free(argv[w+1]);
                }

                exit(0);
            } else{
                waitpid(child, &status, 0);
            }
        }
    }

    free_multiplication_triples(&triples);

    struct rusage resource_usage;
    getrusage(RUSAGE_CHILDREN, &resource_usage);

    printf("user CPU: %f, system CPU: %f, maxrss: %li, ixrss: %li, idrss: %li, "
           "isrss: %li, minflt: %li, majflt %li, nswap: %li, inblock: %li, "
           "oublock: %li, msgsnd: %li, msgrcv: %li, nsignals: %li, nvcsw: %li, "
           "nivcsw: %li", resource_usage.ru_utime.tv_sec + resource_usage.ru_utime.tv_usec/1e6,
           resource_usage.ru_stime.tv_sec + resource_usage.ru_stime.tv_usec/1e6, resource_usage.ru_maxrss,
           resource_usage.ru_ixrss, resource_usage.ru_idrss, resource_usage.ru_isrss, resource_usage.ru_minflt,
           resource_usage.ru_majflt, resource_usage.ru_nswap, resource_usage.ru_inblock, resource_usage.ru_oublock,
           resource_usage.ru_msgsnd, resource_usage.ru_msgrcv, resource_usage.ru_nsignals, resource_usage.ru_nvcsw,
           resource_usage.ru_nivcsw);

    return 0;
}