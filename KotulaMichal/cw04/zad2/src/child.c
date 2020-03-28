#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

enum check_type
{
    ignore,
    handler,
    mask,
    pending
};

void handle_signal()
{
    printf("SIGUSR1 received pid=%i\n", getpid());
}

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        return 0;
    }

    char* check_type_str = argv[1];
    enum check_type check_type;
    if(strcmp(check_type_str, "ignore") == 0)
    {
        check_type = ignore;
    }
    else if(strcmp(check_type_str, "handler") == 0)
    {
        check_type = handler;
    }
    else if(strcmp(check_type_str, "mask") == 0)
    {
        check_type = mask;
    }
    else if(strcmp(check_type_str, "pending") == 0)
    {
        check_type = pending;
    }
    else{
        return 0;
    }

    pid_t child;

    switch(check_type)
    {
        case ignore:
        {
            raise(SIGINT);
            sigset_t pending;
            sigemptyset(&pending);
            sigpending(&pending);
            printf("Zignorowany w potomku? = %i\n", !sigismember(&pending, SIGUSR1));
        }
            break;
        case handler:
            raise(SIGUSR1);
            break;
        case mask:
        {
            sigset_t pending;
            raise(SIGUSR1);
            sigemptyset(&pending);
            sigpending(&pending);
            printf("Widoczny w potomku=%i\n",sigismember(&pending, SIGUSR1));
        }
        break;
        case pending:
        {
            sigset_t pending;
            sigemptyset(&pending);
            sigpending(&pending);
            printf("Widoczny w potomku=%i\n", sigismember(&pending, SIGUSR1));
        }
            break;
    }

    return 0;
}