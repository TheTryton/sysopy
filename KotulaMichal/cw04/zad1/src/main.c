#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>

bool wait_for_decision = false;
bool stop = false;

void handle_terminal_interrupt_signal(int signum) {
    if(wait_for_decision)
    {
        stop = true;
    } else{
        printf("Odebrano sygnal SIGINT\n");
        stop = true;
    }
};

void handle_terminal_stop_signal()
{
    wait_for_decision = !wait_for_decision;

    if(wait_for_decision)
    {
        printf("Oczekuje na CTRL+Z - kontynuacja albo CTRL+C - zakonczenie programu\n");
    }
}

int main()
{
    struct sigaction sa_SIGSTP;
    sa_SIGSTP.sa_handler = handle_terminal_stop_signal;
    sigemptyset(&sa_SIGSTP.sa_mask);
    sa_SIGSTP.sa_flags = 0;
    sigaction(SIGTSTP, &sa_SIGSTP, NULL);

    signal(SIGINT, handle_terminal_interrupt_signal);

    while(!stop)
    {
        if(wait_for_decision)
        {
            pause();
            if(stop)
            {
                break;
            }
        }
        system("ls");
        sleep(1);
    }

    return 0;
}