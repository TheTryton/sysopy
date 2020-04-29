#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/sem.h>
#include <sys/time.h>

#define KEY_PATHNAME "/home/michal/"
#define PROJECT_ID 'S'
#define PERMISSIONS 0600

#include <pthread.h>
#include <sys/errno.h>

typedef struct semaphore
{
    int semaphore_id;
    int semaphore_num;
} semaphore;

int lock(semaphore semaphore)
{
    struct sembuf wait_increment[2] = {{semaphore.semaphore_num, 0, SEM_UNDO}, {semaphore.semaphore_num, 1, SEM_UNDO}};
    return semop(semaphore.semaphore_id, wait_increment, 2);
}

int unlock(semaphore semaphore)
{
    struct sembuf semaphor_decrement = {semaphore.semaphore_num, -1, SEM_UNDO};
    return semop(semaphore.semaphore_id, &semaphor_decrement, 1);
}

typedef enum error_code {
    none = 0,
    deadlock = EDEADLK,
    invalid_value = EINVAL,
    no_thread = ESRCH,
    insufficent_resources = EAGAIN,
    no_permission = EPERM
} error_code;

const char* translate_error(error_code e)
{
    switch(e)
    {
        case none:
            return "No error.\n";
        case deadlock:
            return "Deadlock detected.\n";
        case invalid_value:
            return "Thread is not joinable/another thread is waiting to join with this thread.\n";
        case no_thread:
            return "Invalid thread ID.\n";
        case insufficent_resources:
            return "Insufficent resources to create new thread.\n";
        case no_permission:
            return "No permission to set scheduling policy and parameters attributes when creating thread.\n";
        default:
            return "Invalid error code.\n";
    }
}

typedef struct thread
{
    pthread_t id;
} thread;

typedef enum join_state
{
    exited,
    canceled,
    error
} join_state;

typedef union join_result
{
    int exit_code;
    error_code error_code;
} join_result;

typedef struct init_main
{
    int(*main)(size_t);
    size_t index;
} init_main;

void* tthread_main(void* arg)
{
    int* result = calloc(1, sizeof(int));
    init_main init = *(init_main*)arg;
    free(arg);
    *result = init.main(init.index);
    return result;
}

error_code tcreate(thread* t, int(*thread_main)(size_t), size_t index)
{
    init_main* arg = (init_main*)calloc(1, sizeof(init_main));
    arg->main = thread_main;
    arg->index = index;
    return pthread_create(&t->id, NULL, tthread_main, arg);
}

join_state tjoin(thread* t, join_result* result)
{
    void* join_result;

    error_code err;
    if((err = pthread_join(t->id, &join_result)) != 0)
    {
        if(result)
        {
            result->error_code = err;
        }
        return error;
    } else{
        if(join_result == PTHREAD_CANCELED)
        {
            return canceled;
        } else{
            if(result)
            {
                result->exit_code = *(int*)join_result;
            }
            free(join_result);
            return exited;
        }
    }
}

thread tthis_thread()
{
    thread tt = {pthread_self()};
    return tt;
}

void texit(int* value)
{
    pthread_exit(value);
}

typedef enum stop_reaction
{
    instant = PTHREAD_CANCEL_ASYNCHRONOUS,
    deferred = PTHREAD_CANCEL_DEFERRED
} stop_reaction;

error_code tstop(thread* t)
{
    return pthread_cancel(t->id);
}

void tset_stop_reaction(stop_reaction reaction)
{
    int dummy;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &dummy);
    pthread_setcanceltype(reaction, &dummy);
}

typedef enum division_mode
{
    sign,
    block,
    interleaved
} division_mode;

typedef struct asciipgm_image
{
    u_int8_t** data;
    size_t width, height;
} asciipgm_image;

asciipgm_image load_image(const char* file_name)
{
    asciipgm_image img = {NULL,0,0};

    FILE* image_file = fopen(file_name, "r");

    if(!image_file)
    {
        return img;
    }

    char file_type[129];
    fscanf(image_file, "%128s", file_type);

    if(strcmp(file_type, "P2") != 0)
    {
        return img;
    }

    fscanf(image_file, "%lu %lu", &img.width, &img.height);

    fscanf(image_file, "%128s", file_type); // dummy M

    img.data = (u_int8_t**)calloc(img.width, sizeof(u_int8_t*));
    for(size_t c = 0; c<img.width; c++)
    {
        img.data[c] = (u_int8_t*)calloc(img.height, sizeof(u_int8_t));
    }

    for(size_t c = 0; c<img.width; c++)
    {
        for(size_t r = 0; r<img.height; r++)
        {
            fscanf(image_file, "%hhu", &img.data[c][r]);
        }
    }

    fclose(image_file);

    return img;
}

void free_asciipgm_image(asciipgm_image* img)
{
    for(size_t c = 0; c<img->width; c++)
    {
        free(img->data[c]);
    }
    free(img->data);
    img->data = NULL;
    img->width = 0;
    img->height = 0;
}

division_mode strto_division_mode(const char* str)
{
    if(strcmp(str, "sign") == 0)
    {
        return sign;
    }
    if(strcmp(str, "block") == 0)
    {
        return block;
    }
    if(strcmp(str, "interleaved") == 0)
    {
        return interleaved;
    }
    return sign;
}

void save_histogram(const char* file_name, size_t histogram[256])
{
    FILE* output_file = fopen(file_name, "w+");

    if(!output_file)
    {
        return;
    }

    for(size_t i=0; i<256; i++)
    {
        fprintf(output_file, "%lu %lu\n", i, histogram[i]);
    }

    fclose(output_file);
}

size_t thread_count;
division_mode mode;
const char* input_image_file_name;
const char* output_file_name;
size_t histogram[256];
semaphore histogram_semaphore;
asciipgm_image image;

int get_time()
{
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return (int)(currentTime.tv_sec * (int)1e6 + currentTime.tv_usec);
}

int thread_func(size_t index)
{
    int start_time = get_time();

    switch(mode)
    {
        default:
        case sign:
        {
            u_int8_t color_start = 256.0 / thread_count * index;
            u_int8_t color_end = 256.0 /thread_count * (double)(index + 1);

            for(size_t c = 0; c < image.width; c++) {
                for (size_t r = 0; r < image.height; r++) {
                    // theoretically reading from memory concurrently
                    // is not allowed without exclusion lock on ARM
                    // architecture, this doesn't concern us because we are
                    // on x86 architecture which allows it

                    if(image.data[c][r] < color_end && image.data[c][r] >= color_start)
                    {
                        histogram[image.data[c][r]]++;
                    }
                }
            }
        }
        break;
        case block:
        {
            size_t local_histogram[256];

            size_t x_start = (double)image.width / thread_count * index;
            size_t x_end = (double)image.width / thread_count * (double)(index + 1);

            for(size_t c = x_start; c < x_end; c++) {
                for (size_t r = 0; r < image.height; r++) {
                    // theoretically reading from memory concurrently
                    // is not allowed without exclusion lock on ARM
                    // architecture, this doesn't concern us because we are
                    // on x86 architecture which allows it

                    local_histogram[image.data[c][r]]++;
                }
            }

            lock(histogram_semaphore);
            for(size_t i=0; i<256; i++)
            {
                histogram[i]+=local_histogram[i];
            }
            unlock(histogram_semaphore);
        }
        break;
        case interleaved:
        {
            size_t local_histogram[256];

            for(size_t c = index; c < image.width; c+=thread_count) {
                for (size_t r = 0; r < image.height; r++) {
                    // theoretically reading from memory concurrently
                    // is not allowed without exclusion lock on ARM
                    // architecture, this doesn't concern us because we are
                    // on x86 architecture which allows it

                    local_histogram[image.data[c][r]]++;
                }
            }

            lock(histogram_semaphore);
            for(size_t i=0; i<256; i++)
            {
                histogram[i]+=local_histogram[i];
            }
            unlock(histogram_semaphore);
        }
        break;
    }

    int end_time = get_time();

    return end_time - start_time;
}

int main(int argc, char** argv) {
    if(argc != 5)
    {
        printf("zad1 thread_count division_mode(sign/block/interleaved) input_image_file_name output_file_name\n");
        return 0;
    }

    thread_count = strtoll(argv[1], NULL, 10);
    mode = strto_division_mode(argv[2]);
    input_image_file_name = argv[3];
    output_file_name = argv[4];

    int start = get_time();

    image = load_image(input_image_file_name);

    thread threads[thread_count];

    if(image.data)
    {
        key_t key = ftok(KEY_PATHNAME, PROJECT_ID);

        int semaphore_id = semget(key, 1, IPC_CREAT | IPC_EXCL | PERMISSIONS);

        if(semaphore_id == -1)
        {
            printf("Failed to create semaphore.\n");
            return 0;
        }

        semaphore sem = {semaphore_id, 0};
        histogram_semaphore = sem;

        for(size_t i=0; i<256; i++)
        {
            histogram[i]=0;
        }

        for(size_t i=0;i<thread_count;i++)
        {
           translate_error(tcreate(&threads[i], thread_func, i));
        }

        for(size_t i=0;i<thread_count;i++)
        {
            join_result res;

            translate_error(tjoin(&threads[i], &res));

            printf("Thread %lu took %i us to compute histogram part.\n", i, res.exit_code);
        }

        semctl(histogram_semaphore.semaphore_id, histogram_semaphore.semaphore_num, IPC_RMID, NULL);

        save_histogram(output_file_name, histogram);

        free_asciipgm_image(&image);

        int end = get_time();

        printf("Whole process took %i us to compute histogram.\n", end - start);
    }

    return 0;
}