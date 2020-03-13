#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

void generate_record(char* record, int record_length)
{
    for(int i=0;i<record_length;i++)
    {
        record[i] = rand() % ('z'-'a'+1)+'a';
    }
}

typedef struct offset_size_sys
{
    int offset;
    int size;
    int file_descriptor;
} offset_size_sys;

typedef struct offset_size_lib
{
    int offset;
    int size;
    FILE* file;
} offset_size_lib;

int lexicographic_file_compare_sys(const void* va, const void* vb)
{
    offset_size_sys osa = *(const offset_size_sys*)va;
    offset_size_sys osb = *(const offset_size_sys*)vb;
    char* a = calloc(osa.size + 1, sizeof(char));
    a[osa.size] = '\0';
    char* b = calloc(osb.size + 1, sizeof(char));
    b[osb.size] = '\0';

    lseek(osa.file_descriptor, osa.offset, SEEK_SET);
    read(osa.file_descriptor, a, osa.size);

    lseek(osb.file_descriptor, osb.offset, SEEK_SET);
    read(osb.file_descriptor, b, osb.size);

    int res = strcmp(a,b);
    free(a);
    free(b);
    return res;
}

int lexicographic_file_compare_lib(const void* va, const void* vb)
{
    offset_size_lib osa = *(const offset_size_lib*)va;
    offset_size_lib osb = *(const offset_size_lib*)vb;
    char* a = calloc(osa.size + 1, sizeof(char));
    a[osa.size] = '\0';
    char* b = calloc(osb.size + 1, sizeof(char));
    b[osb.size] = '\0';

    fseek(osa.file, osa.offset, 0);
    fread(osa.file, sizeof(char), a, osa.size);

    fseek(osb.file, osb.offset, 0);
    fread(osb.file, sizeof(char), b, osb.size);

    int res = strcmp(a,b);
    free(a);
    free(b);
    return res;
}

int sort_file_sys(const char* file1, int records_count, int record_length)
{
    int descriptor = open(file1,O_RDWR);
    if(descriptor)
    {
        offset_size_sys* offsets = calloc(records_count, sizeof(offset_size_sys));
        for(int i=0;i<records_count;i++)
        {
            offsets[i].offset = i*record_length;
            offsets[i].size = record_length;
            offsets[i].file_descriptor = descriptor;
        }
        qsort(offsets, records_count, sizeof(offset_size_sys), lexicographic_file_compare_sys);
        free(offsets);
        close(descriptor);
        return 0;
    } else{
        return -1;
    }
}

int sort_file_lib(const char* file1, int records_count, int record_length)
{
    FILE* file = fopen(file1,"r+");
    if(file)
    {
        offset_size_lib* offsets = calloc(records_count, sizeof(offset_size_lib));
        for(int i=0;i<records_count;i++)
        {
            offsets[i].offset = i*record_length;
            offsets[i].size = record_length;
            offsets[i].file = file;
        }
        qsort(offsets, records_count, sizeof(offset_size_lib), lexicographic_file_compare_lib);
        free(offsets);
        fclose(file);
        return 0;
    } else{
        return -1;
    }
}

int find_lf(char* buffer, int offset, int size)
{
    for(int i=offset;i<size;i++)
    {
        if(buffer[i]=='\n')
        {
            return i;
        }
    }
    return -1;
}

int copy_sys(const char* file_src, const char* file_dst, int records_count, int buffer_size)
{
    int descriptor_src = open(file_src, O_RDONLY);
    int descriptor_dst = open(file_dst, O_WRONLY);
    if(descriptor_src && descriptor_dst)
    {
        char* buffer = calloc(buffer_size, sizeof(char));
        int real_read = 0;
        int read_total = 0;
        int read_records = 0;
        while((real_read = read(descriptor_src, buffer, buffer_size)) != 0 && read_records != records_count)
        {
            int offset = 0;
            int clamp = real_read;
            while((offset = find_lf(buffer, offset, buffer_size)) != -1)
            {
                read_records++;
                if(read_records == records_count)
                {
                    clamp = offset;
                    break;
                }
            }
            write(file_dst, buffer, clamp);
            read_total += real_read;
        }
        free(buffer);
        close(descriptor_src);
        close(descriptor_dst);
        return 0;
    } else {
        return -1;
    }
}

int copy_lib(const char* filen_src, const char* filen_dst, int records_count, int buffer_size)
{
    FILE* file_src = fopen(filen_src, "r+");
    FILE* file_dst = fopen(filen_dst, "w+");
    if(file_src && file_dst)
    {
        char* buffer = calloc(buffer_size, sizeof(char));
        int real_read = 0;
        int read_total = 0;
        int read_records = 0;
        while((real_read = fread(file_src,sizeof(char), buffer, buffer_size)) != 0 && read_records != records_count)
        {
            int offset = 0;
            int clamp = real_read;
            while((offset = find_lf(buffer, offset, buffer_size)) != -1)
            {
                read_records++;
                if(read_records == records_count)
                {
                    clamp = offset;
                    break;
                }
            }
            fwrite(file_dst, sizeof(char), buffer, clamp);
            read_total += real_read;
        }
        free(buffer);
        fclose(file_src);
        fclose(file_dst);
        return 0;
    } else {
        return -1;
    }
}

int main(int argc, char** argv)
{
    if(argc >= 2)
    {
        char* operation = argv[1];

        if(strcmp(operation, "generate") == 0)
        {
            if(argc < 5)
            {
                printf("generate <filename> <records count> <record length> <sys/lib>");
                return 0;
            }

            char* filename = argv[2];
            int records_count = atoi(argv[3]);
            int record_length = atoi(argv[4]);
            char* library_type = argc >= 6 ? argv[5] : "sys";

            if(strcmp(library_type,"sys") == 0)
            {
                int descriptor = open(filename, O_WRONLY | O_CREAT);
                if(descriptor)
                {
                    char* record = calloc(record_length, sizeof(char));

                    for(int i=0;i<records_count;i++)
                    {
                        generate_record(record, record_length);
                        write(descriptor, record, record_length);
                        write(descriptor, "\n", 1);
                    }
                    free(record);

                    close(descriptor);
                } else{
                    printf("Failed to open file to generate records");
                    return 0;
                }
            }
            else if(strcmp(library_type,"lib") == 0)
            {
                FILE* file = fopen(filename, "w+");
                if(file)
                {
                    char* record = calloc(record_length, sizeof(char));

                    for(int i=0;i<records_count;i++)
                    {
                        generate_record(record, record_length);
                        fwrite(record, sizeof(char), record_length, file);
                        fwrite("\n",sizeof(char),1,file);
                    }
                    free(record);

                    fclose(file);
                } else{
                    printf("Failed to open file to generate records");
                    return 0;
                }
            }
            else{
                printf("library type must be lib or sys");
                return 0;
            }
        }
        else if(strcmp(operation, "sort") == 0)
        {

        }
        else if(strcmp(operation, "copy") == 0)
        {

        } else{
            printf("invalid operation (generate/sort/copy)");
            return 0;
        }
    }
}