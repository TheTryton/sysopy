#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>

void handler(int sig, siginfo_t *info, void *ucontext)
{
    if(sig == SIGSEGV)
    {
        printf("Segmentation fault\n");
        printf("Access violation reading location: %p\n", info->si_addr);
        // signal automatically raised when user dereferences memory address that does not
        // belong to this program (or the program has no access to this memory location)
        // signal is raised "before" operation -> so if we dont fix the problematic line
        // the signal will be reraised
        // si_addr - memory location that caused fault
        exit(0);
    }
    else if(sig == SIGCHLD)
    {
        printf("Child has terminated\n");
        printf("Child had pid=%i\n", info->si_pid);
        // signal automatically raised when a child process has finished
        // si_pid contains pid of the child that was terminated
        printf("Child has returned with %i\n", info->si_status);
        // si_status contains exit status of child
        printf("Child used %li user time\n", info->si_utime);
        printf("Child used %li system time\n", info->si_utime);
        // si_utime/si_stime contains the user and system time used by the child process
    }
    else if(sig == SIGFPE)
    {
        printf("Floating point exception\n");
        printf("At location: %p\n", info->si_addr);
        exit(12);
    }
}

int main()
{
    struct sigaction ac;
    ac.sa_flags = SA_SIGINFO;
    ac.sa_sigaction = handler;
    sigemptyset(&ac.sa_mask);

    sigaction(SIGSEGV, &ac, NULL);
    sigaction(SIGCHLD, &ac, NULL);
    sigaction(SIGFPE, &ac, NULL);

    pid_t child;
    if((child = fork()) == 0)
    {
        printf("Child started\n");

        //trigger SIGFPE
        int a = 1;
        int b = 0;
        int c = a/b;
    } else{
        printf("Created child with pid=%i\n",child);
    }

    waitpid(child,NULL, 0);

    //trigger SIGSEGV
    int* ptr=(int*)0xffaa;
    int v = *ptr;

    return 0;
}