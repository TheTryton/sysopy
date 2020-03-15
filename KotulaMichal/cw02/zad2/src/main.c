#define _XOPEN_SOURCE 600
#define _GNU_SOURCE

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <assert.h>
#include <sys/stat.h>
#include <time.h>
#include <ftw.h>

enum set_value
{
    not_set,
    value_set,
    set_greater,
    set_less
};

typedef struct input_arguments
{
    int mtime;
    int mtime_set;
    int atime;
    int atime_set;
    int maxdepth;
    int maxdepth_set;
    const char* dir_path;
} input_arguments;

const char* decode_file_type(struct stat file_statistics)
{
    if(S_ISREG(file_statistics.st_mode))
    {
        return "file";
    }
    if(S_ISDIR(file_statistics.st_mode))
    {
        return "dir";
    }
    if(S_ISCHR(file_statistics.st_mode))
    {
        return "char dev";
    }
    if(S_ISBLK(file_statistics.st_mode))
    {
        return "block dev";
    }
    if(S_ISFIFO(file_statistics.st_mode))
    {
        return "fifo";
    }
    if(S_ISLNK(file_statistics.st_mode))
    {
        return "slink";
    }
    if(S_ISSOCK(file_statistics.st_mode))
    {
        return "sock";
    }

    return "";
}

char* format_time(const time_t* time)
{
    struct tm* ptm = localtime(time);
    char* time_buffer = calloc(64, sizeof(char));
    strftime(time_buffer, 64, "%a, %d.%m.%Y %H:%M:%S", ptm);
    return time_buffer;
}

int days_ago(time_t t)
{
    return difftime(time(NULL), t)/86400;
}

int filter_file(struct stat file_statistics, input_arguments args)
{
    if(args.mtime_set == set_greater && days_ago(file_statistics.st_mtim.tv_sec) <= args.mtime)
    {
        return 1;
    }
    if(args.mtime_set == set_less && days_ago(file_statistics.st_mtim.tv_sec) >= args.mtime)
    {
        return 1;
    }
    if(args.mtime_set == value_set && days_ago(file_statistics.st_mtim.tv_sec) != args.mtime)
    {
        return 1;
    }

    if(args.atime_set == set_greater && days_ago(file_statistics.st_atim.tv_sec) <= args.atime)
    {
        return 1;
    }
    if(args.atime_set == set_less && days_ago(file_statistics.st_atim.tv_sec) >= args.atime)
    {
        return 1;
    }
    if(args.atime_set == value_set && days_ago(file_statistics.st_atim.tv_sec) != args.atime)
    {
        return 1;
    }

    return 0;
}

void print_file_info_a(const char* path, struct stat file_statistics)
{
    // file absolute path
    char fullpath[PATH_MAX+1];
    realpath(path, fullpath);
    printf("%s ", fullpath);

    // hardlinks count

    printf("hardlinks_count=%lu ", file_statistics.st_nlink);

    // file type

    printf("file_type=%s ",  decode_file_type(file_statistics));

    // file size

    printf("file_size=%li ", file_statistics.st_size);

    // last access time

    char* formatted_access_time = format_time(&file_statistics.st_atim.tv_sec);

    printf("last_access_time=%s ", formatted_access_time);

    free(formatted_access_time);

    // last modification time

    char* formatted_modification_time = format_time(&file_statistics.st_mtim.tv_sec);

    printf("last_modification_time=%s\n", formatted_modification_time);

    free(formatted_modification_time);
}

void traverse_a_impl(const char* dir_path, int current_depth, input_arguments args)
{
    // jump out if reached maxdepth
    if(args.maxdepth_set == value_set && current_depth > args.maxdepth)
    {
        return;
    }

    // open directory

    DIR* dir;
    struct dirent* entry;

    if(!(dir = opendir(dir_path)))
        return;

    while((entry = readdir(dir)) != NULL)
    {
        // ignore current directory and parent directory

        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // create relative path to the root

        char path[PATH_MAX+1];
        snprintf(path, PATH_MAX+1, "%s/%s", dir_path, entry->d_name);

        // get file statistics

        struct stat file_statistics;
        lstat(path, &file_statistics);

        // ignore file if it doesnt satisfy either mtime or atime

        if(!filter_file(file_statistics, args))
        {
            // print complete file info
            print_file_info_a(path, file_statistics);
        }

        // if file is a directory, traverse recursively

        if(entry->d_type == DT_DIR)
        {
            traverse_a_impl(path, current_depth + 1, args);
        }
    }

}

void traverse_a(input_arguments args)
{
    struct stat file_statistics;
    lstat(args.dir_path, &file_statistics);
    if(!filter_file(file_statistics, args))
    {
        print_file_info_a(args.dir_path, file_statistics);
    }
    traverse_a_impl(args.dir_path, 1, args);
}

int calculate_depth(const char* path)
{
    const char* c = path;
    int depth = 0;
    while(*c != '\0')
    {
        if(*c == '/')
        {
            depth++;
        }
        c++;
    }
    return depth;
}

input_arguments traverse_b_input_arguments;

int traverse_b_impl(const char* path, const struct stat* fs, int typeflag, struct FTW* ftwbuf)
{
    if(traverse_b_input_arguments.maxdepth_set == value_set && calculate_depth(path) > traverse_b_input_arguments.maxdepth)
    {
        return FTW_SKIP_SUBTREE;
    }

    // get file statistics

    struct stat file_statistics;
    lstat(path, &file_statistics);

    // ignore file if it doesnt satisfy either mtime or atime

    if(!filter_file(file_statistics, traverse_b_input_arguments))
    {
        // print complete file info
        print_file_info_a(path, file_statistics);
    }

    return FTW_CONTINUE;
}

void traverse_b(input_arguments args)
{
    traverse_b_input_arguments = args;
    nftw(args.dir_path, traverse_b_impl, 1, FTW_ACTIONRETVAL | FTW_PHYS);
    //ftw(args.dir_path, traverse_b_impl, 1);
}

input_arguments parse_arguments(int argc, char** argv)
{
    input_arguments args;
    args.atime_set = not_set;
    args.mtime_set = not_set;
    args.maxdepth_set = not_set;
    args.dir_path = ".";

    int current_index = 1;
    while(current_index < argc)
    {
        if(strcmp(argv[current_index], "-maxdepth") == 0)
        {
            current_index++;
            assert(current_index < argc && "-maxdepth n");
            args.maxdepth = atoi(argv[current_index]);
            args.maxdepth_set = value_set;
            current_index++;
            continue;
        }

        if(strcmp(argv[current_index], "-atime") == 0)
        {
            current_index++;
            assert(current_index < argc && "-atime [+|-]n");
            switch(argv[current_index][0])
            {
                case '+':
                    args.atime_set = set_greater;
                    break;
                case '-':
                    args.atime_set = set_less;
                    break;
                default:
                    args.atime_set = value_set;
                    break;
            }
            args.atime = atoi(argv[current_index] + ((args.atime_set == value_set) ? 0 : 1));
            current_index++;
            continue;
        }

        if(strcmp(argv[current_index], "-mtime") == 0)
        {
            current_index++;
            assert(current_index < argc && "-mtime [+|-]n");
            switch(argv[current_index][0])
            {
                case '+':
                    args.mtime_set = set_greater;
                    break;
                case '-':
                    args.mtime_set = set_less;
                    break;
                default:
                    args.mtime_set = value_set;
                    break;
            }
            args.mtime = atoi(argv[current_index] + ((args.mtime_set == value_set) ? 0 : 1));
            current_index++;
            continue;
        }

        if(argv[current_index][0] != '-')
        {
            args.dir_path = argv[current_index];
            current_index++;
            continue;
        }
    }

    return args;
}

#ifndef USE_B_TRAVERSE
#define USE_B_TRAVERSE 1
#endif
#include <features.h>
int main(int argc, char** argv)
{
    if(USE_B_TRAVERSE)
    {
        traverse_b(parse_arguments(argc, argv));
    } else{
        traverse_a(parse_arguments(argc, argv));
    }

    return 0;
}
