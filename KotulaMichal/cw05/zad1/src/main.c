#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>

#define MAX_LINE_COMMAND_LENGTH 512
#define MAX_TOKENS 64

typedef struct command
{
    char* program;
    char** arguments;
    size_t arguments_count;
} command;

typedef struct commands
{
    command* comms;
    size_t commands_count;
} commands;

void print_help()
{
    printf("zad1 file_path");
}

command parse_command(int argc, char** argv, int* offset)
{
    command comm;

    comm.program = calloc(strlen(argv[*offset])+1,sizeof(char));
    strcpy(comm.program, argv[*offset]);
    (*offset)++;

    int args = 0;
    while(*offset + args < argc && strcmp(argv[*offset + args], "|") != 0)
    {
        args++;
    }

    comm.arguments = calloc(args + 2, sizeof(char*));
    comm.arguments_count = args + 1;
    comm.arguments[0] =  calloc(strlen(comm.program)+1,sizeof(char));
    strcpy(comm.arguments[0], comm.program);

    for(size_t i=1;i<=args;i++)
    {
        comm.arguments[i] = calloc(strlen(argv[*offset + i - 1]) + 1, sizeof(char));
        strcpy(comm.arguments[i], argv[*offset + i - 1]);
    }

    comm.arguments[args + 1] = NULL;

    (*offset) += args;

    return comm;
}

size_t count_commands(int argc, char** argv)
{
    size_t commands_count = argc > 1;
    for(size_t i=1;i<argc;i++)
    {
        if(strcmp(argv[i], "|") == 0)
        {
            commands_count++;
        }
    }
    return commands_count;
}

commands parse_commands(int argc, char** argv)
{
    commands comms;
    comms.commands_count = count_commands(argc, argv);
    comms.comms = calloc(comms.commands_count, sizeof(command));
    int offset = 0;
    int command = 0;
    while(offset < argc)
    {
        comms.comms[command++] = parse_command(argc, argv, &offset);
        offset++; //skip | character
    }
    return comms;
}

void free_command(command* comm)
{
    free(comm->program);
    comm->program = NULL;
    for(size_t i=0;i<comm->arguments_count;i++)
    {
        free(comm->arguments[i]);
    }
    free(comm->arguments);
    comm->arguments = NULL;
    comm->arguments_count=0;
}

void free_commands(commands* comms)
{
    for(size_t i=0;i<comms->commands_count;i++)
    {
        free_command(&comms->comms[i]);
    }
    free(comms->comms);
    comms->comms = NULL;
    comms->commands_count = 0;
}

typedef enum mode
{
    none = 0,
    output = 1,
    input = 2,
    io_from_pipe = 3
} mode;

pid_t exec_command(command comm, mode m, int prev_fd[2], int curr_fd[2])
{
    if(pipe(curr_fd) != 0)
    {
        printf("pipe() error\n");
        exit(1);
    }

    pid_t child = fork();
    if(child == 0)
    {
        if(m & output) // should redirect stdout
        {
            close(curr_fd[0]);
            if(dup2(curr_fd[1], STDOUT_FILENO) == -1)
            {
                printf("dup2() error\n");
                exit(1);
            }
        }

        if(m & input) // should redirect stdin
        {
            close(prev_fd[1]);
            if(dup2(prev_fd[0], STDIN_FILENO) == -1)
            {
                printf("dup2() error\n");
                exit(1);
            }
        }

        if(execvp(comm.program, comm.arguments) == -1)
        {
            printf("execvp() error\n");
            exit(1);
        }
    }

    if(m & io_from_pipe) // io comes from redirected stdout/in
    {
        close(prev_fd[0]);
        close(prev_fd[1]);
    }

    if(m & output) // redirects output
    {
        prev_fd[0] = curr_fd[0];
        prev_fd[1] = curr_fd[1];
    }
    else // does not redirect output
    {
        //finish pipe'ing
        close(curr_fd[0]);
        close(curr_fd[1]);
    }

    return child;
}

typedef struct tokenized_command
{
    int argc;
    char** argv;
} tokenized_command;

tokenized_command tokenize_command(char* unparsed_command)
{
    tokenized_command comm;
    comm.argc = 0;
    comm.argv = calloc(MAX_TOKENS, sizeof(char*));
    char* arg = strtok(unparsed_command, " \n");

    while(arg != NULL)
    {
        comm.argv[comm.argc++] = arg;
        arg = strtok(NULL, " \n");
    }

    return comm;
}

void free_tokenized_command(tokenized_command* tc)
{
    free(tc->argv);
    tc->argv = NULL;
    tc->argc = 0;
}

void parse_and_execute_line_command(char* unparsed_command)
{
    tokenized_command tcomm = tokenize_command(unparsed_command);

    commands commands = parse_commands(tcomm.argc, tcomm.argv);

    int prev_fd[2];
    int curr_fd[2];

    for(size_t i = 0; i < commands.commands_count; i++)
    {
        mode m = none;
        m |= (i != 0 ? input : 0);
        m |= (i != commands.commands_count - 1 ? output : 0);
        pid_t child = exec_command(commands.comms[i], m, prev_fd, curr_fd);
        // at the end we have to wait until last command process has finished working
        if(i == commands.commands_count - 1)
        {
            waitpid(child, NULL, 0);
        }
    }

    free_commands(&commands);

    free_tokenized_command(&tcomm);
}

int main(int argc, char** argv)
{
    if(argc < 2 || strcmp(argv[1],"-h") == 0)
    {
        print_help();
    } else{
        char* file_path = argv[1];

        FILE* file = fopen(file_path, "r");
        if(file == NULL)
        {
            return 1;
        }

        char unparsed_command[MAX_LINE_COMMAND_LENGTH];

        while(fgets(unparsed_command, MAX_LINE_COMMAND_LENGTH, file) != NULL) // read line by line
        {
            parse_and_execute_line_command(unparsed_command);
        }

        fclose(file);
    }

    return 0;
}