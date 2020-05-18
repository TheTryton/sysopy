#pragma once

#include <fcntl.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>

#include <sys/epoll.h>

#include <signal.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define exit_on_error(expr) \
    ({\
        typeof(expr) _tmp = (expr);\
        if(_tmp == -1){\
            printf(#expr" failed: %s\n", strerror(errno));\
            exit(EXIT_FAILURE);\
        }\
        _tmp;\
     })

#define find(init, cond) ({ int index = -1;  for (init) if (cond) { index = i; break; } index; })