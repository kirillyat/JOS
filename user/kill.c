#include <inc/lib.h>
#include <inc/env.h>
#include <inc/signal.h>

void
umain(int argc, char **argv) 
{
    binaryname = "kill";
    int signo;
    int value;
    envid_t envid;
    switch (argc)
    {
    case 1:
    case 2:
    cprintf("Usage: %s -[SIGNO] ([VALUE] [ENVID]) | [ENVID]\n", argv[0]);
    cprintf("Send a signal to a process.\n");
    break;
    case 3:
        signo = -strtol(argv[1], 0, 10);
        if (signo < 0) break;
        envid = strtol(argv[2], 0, 16);
        sys_sigqueue(envid, signo, 0);
        break;
    case 4:
        signo = -strtol(argv[1], 0, 10);
        if (signo < 0) break;
        value = strtol(argv[2], 0, 10);
        envid = strtol(argv[3], 0, 16);
        sys_sigqueue( envid, signo, value);
        break;
    default:
        break;
    };
}