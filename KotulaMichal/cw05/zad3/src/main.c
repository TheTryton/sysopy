#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{
    if(fork() == 0)
    {
        execlp("./zad3consumer","./zad3consumer", "prod_cons", "output", "512", NULL);
    }
    else if(fork() == 0)
    {
        execlp("./zad3producer","./zad3producer", "prod_cons", "a", "512", NULL);
    }
    else if(fork() == 0)
    {
        execlp("./zad3producer","./zad3producer", "prod_cons", "b", "512", NULL);
    }
    else if(fork() == 0)
    {
        execlp("./zad3producer","./zad3producer", "prod_cons", "c", "512", NULL);
    }
    else if(fork() == 0)
    {
        execlp("./zad3producer","./zad3producer", "prod_cons", "d", "512", NULL);
    }
    else if(fork() == 0)
    {
        execlp("./zad3producer","./zad3producer", "prod_cons", "e", "512", NULL);
    } else{
        pid_t wpid;
        while ((wpid = wait(NULL)) > 0)
        {
            printf("Process %i finished.\n", wpid);
        }
    }

    return 0;
}