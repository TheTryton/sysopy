
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void handle_signal()
{
    printf("Signal SIGUSR1 received in process with pid=%i\n",getpid());
}

enum check_type
{
    ignore,
    handler,
    mask,
    pending
};

int main(int argc, char** argv)
{
    if(argc < 3)
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
    bool use_exec = atoi(argv[2]);

    struct sigaction sa;

    pid_t child;

    switch(check_type)
    {
        case ignore: {
            sa.sa_handler = SIG_IGN;
            sa.sa_flags = 0;
            sigemptyset(&sa.sa_mask);
            sigaction(SIGINT, &sa, NULL);
            raise(SIGINT);
            sigset_t pending;
            sigemptyset(&pending);
            sigpending(&pending);
            printf("Zignorowany w przodku? = %i\n", !sigismember(&pending, SIGUSR1));
            if ((child = fork()) == 0) {
                if (use_exec) {
                    execl("./zad2child", "./zad2child", argv[1], NULL);
                } else {
                    raise(SIGINT);
                    sigset_t pending;
                    sigemptyset(&pending);
                    sigpending(&pending);
                    printf("Zignorowany w potomku? = %i\n", !sigismember(&pending, SIGUSR1));
                }
            }
        }
            break;
        case handler:
            sa.sa_handler = handle_signal;
            sa.sa_flags = 0;
            sigemptyset(&sa.sa_mask);
            sigaction(SIGUSR1, &sa, NULL);
            raise(SIGUSR1);
            if((child = fork()) == 0)
            {
                if(use_exec)
                {
                    execl("./zad2child", "./zad2child", argv[1], NULL);
                } else{
                    raise(SIGUSR1);
                }
            }
            break;
        case mask:
        {
            sigset_t newmask;
            sigset_t oldmask;
            sigemptyset(&newmask);
            sigaddset(&newmask, SIGUSR1);
            sigprocmask(SIG_BLOCK, &newmask, &oldmask);
            raise(SIGUSR1);
            sigset_t pending;
            sigpending(&pending);
            printf("Widoczny w przodku=%i\n",sigismember(&pending, SIGUSR1));
            if((child = fork()) == 0)
            {
                if(use_exec)
                {
                    execl("./zad2child", "./zad2child", argv[1], NULL);
                } else{
                    raise(SIGUSR1);
                    sigemptyset(&pending);
                    sigpending(&pending);
                    printf("Widoczny w potomku=%i\n",sigismember(&pending, SIGUSR1));
                }
            }
        }
        break;
        case pending:
        {
            sigset_t newmask;
            sigset_t oldmask;
            sigemptyset(&newmask);
            sigaddset(&newmask, SIGUSR1);
            sigprocmask(SIG_BLOCK, &newmask, &oldmask);
            raise(SIGUSR1);
            sigset_t pending;
            sigpending(&pending);
            printf("Widoczny w przodku=%i\n",sigismember(&pending, SIGUSR1));
            if((child = fork()) == 0)
            {
                if(use_exec)
                {
                    execl("./zad2child", "./zad2child", argv[1], NULL);
                } else{
                    sigemptyset(&pending);
                    sigpending(&pending);
                    printf("Widoczny w potomku=%i\n",sigismember(&pending, SIGUSR1));
                }
            }
        }
            break;
    }

    if(child != 0)
    {
        waitpid(child, NULL, 0);
    }

    return 0;
}
