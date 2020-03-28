#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

bool finished = false;
int receivedUSR1 = 0;
pid_t sender_pid;

enum send_mode
{
    s_kill,
    s_sigqueue,
    s_sigrt
};

enum send_mode send_mode;

enum signal_num
{
    SIGNAL1,
    SIGNAL2
};

int encode_signal(enum signal_num signal)
{
    switch(send_mode) {
        case s_kill:
        case s_sigqueue:
            return signal == SIGNAL1 ? SIGUSR1 : SIGUSR2;
        case s_sigrt:
            return signal == SIGNAL1 ? SIGRTMIN : SIGRTMIN + 1;
    }
}

enum signal_num decode_signal(int signal)
{
    switch(send_mode) {
        case s_kill:
        case s_sigqueue:
            return signal == SIGUSR1 ? SIGNAL1 : SIGNAL2;
        case s_sigrt:
            return signal == SIGRTMIN ? SIGNAL1 : SIGNAL2;
    }
}

void send_signal(pid_t target, enum signal_num signal)
{
    switch(send_mode)
    {
        case s_kill:
        case s_sigrt:
        {
            kill(target, encode_signal(signal));
        }
            break;
        case s_sigqueue:
        {
            union sigval v;
            sigqueue(target, encode_signal(signal), v);
        }
            break;
    }
}

void handler_unconfirmed(int sig, siginfo_t *info, void *ucontext)
{
    if(decode_signal(sig) == SIGNAL1)
    {
        sender_pid = info->si_pid;
        receivedUSR1++;
    }
    else if(decode_signal(sig) == SIGNAL2)
    {
        finished=true;
    }
}

void block_all_signals()
{
    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    sigemptyset(&mask);
    sigaddset(&mask, encode_signal(SIGNAL1));
    sigaddset(&mask, encode_signal(SIGNAL2));
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

void listen_to_signals(void (*handler) (int, siginfo_t *, void *))
{
    struct sigaction ac;
    ac.sa_sigaction = handler;
    ac.sa_flags = SA_SIGINFO;
    sigemptyset(&ac.sa_mask);

    sigaction(encode_signal(SIGNAL1), &ac, NULL);
    sigaction(encode_signal(SIGNAL2), &ac, NULL);
}

bool sending = false;
bool confirmed = false;

void send_signal_confirmed(pid_t target, enum signal_num signal)
{
    switch(send_mode)
    {
        case s_kill:
        case s_sigrt:
        {
            confirmed=false;
            kill(target, encode_signal(signal));
            while(!confirmed);
        }
            break;
        case s_sigqueue:
        {
            confirmed=false;
            union sigval v;
            sigqueue(target, encode_signal(signal), v);
            while(!confirmed);
        }
            break;
    }
}

void handler_confirmed(int sig, siginfo_t *info, void *ucontext)
{
    if(sending)
    {
        if(decode_signal(sig) == SIGNAL1) // confirmation received
        {
            confirmed = true;
        }
    }
    else
    {
        if(decode_signal(sig) == SIGNAL1)
        {
            sender_pid = info->si_pid;
            receivedUSR1++;
            send_signal(sender_pid, SIGNAL1); // send confirmation
        }
        else if(decode_signal(sig) == SIGNAL2)
        {
            finished=true;
        }
    }
}

int unconfirmed_mode(int argc, char** argv)
{
    if(strcmp(argv[1], "kill") == 0)
    {
        send_mode = s_kill;
    }
    else if(strcmp(argv[1], "sigqueue") == 0)
    {
        send_mode = s_sigqueue;
    }
    else if(strcmp(argv[1], "sigrt") == 0)
    {
        send_mode = s_sigrt;
    }
    else
    {
        return 0;
    }

    printf("PID of catcher = %i\n", getpid());

    block_all_signals();
    listen_to_signals(handler_unconfirmed);

    while(!finished);

    printf("Catcher received %i SIGUSR1 signals\n", receivedUSR1);

    for(int i=0;i<receivedUSR1;i++)
    {
        send_signal(sender_pid, SIGNAL1);
    }

    send_signal(sender_pid, SIGNAL2);

    return 0;
}

int confirmed_mode(int argc, char** argv)
{
    if(strcmp(argv[1], "kill") == 0)
    {
        send_mode = s_kill;
    }
    else if(strcmp(argv[1], "sigqueue") == 0)
    {
        send_mode = s_sigqueue;
    }
    else if(strcmp(argv[1], "sigrt") == 0)
    {
        send_mode = s_sigrt;
    }
    else
    {
        return 0;
    }

    printf("PID of catcher = %i\n", getpid());

    block_all_signals();
    listen_to_signals(handler_confirmed);

    while(!finished);

    printf("Catcher received %i SIGUSR1 signals\n", receivedUSR1);

    sending = true;

    for(int i=0;i<receivedUSR1;i++)
    {
        send_signal_confirmed(sender_pid, SIGNAL1);
    }

    send_signal(sender_pid, SIGNAL2);

    return 0;
}

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        return 0;
    }

    if(argc == 3)
    {
        bool mode = atoi(argv[2]);

        if(mode)
        {
            return confirmed_mode(argc, argv);
        }
        else
        {
            return unconfirmed_mode(argc, argv);
        }
    }
    else
    {
        return unconfirmed_mode(argc, argv);
    }
}