#ifndef JOS_INC_SIGNAL_H
#define JOS_INC_SIGNAL_H

/* signal numbers */
enum
{
    SIG_test = 0,
    SIGHUP   = 1,
    SIGINT   = 2,
    SIGQUIT  = 3,
    SIGILL   = 4,
    SIGABRT  = 6,
    SIGFPE   = 8,
    SIGKILL  = 9,
    SIGUSR1  = 10,
    SIGSEGV  = 11,
    SIGUSR2  = 12,
    SIGPIPE  = 13,
    SIGALRM  = 14,
    SIGTERM  = 15,
    SIGCHLD  = 17,
    SIGCONT  = 18,
    SIGSTOP  = 19,
    SIGTSTP  = 20,
    SIGTTIN  = 21,
    SIGTTOU  = 22,
    NSIGNALS
};

#define SIG_DFL ((void *) 0)
#define SIG_IGN ((void *) 1)

struct sigaction
{
    void (*sa_handler)(int);
    void (*sa_restorer)();
};

struct sigevent
{
    int sigev_signo;
    int sigev_value;
};

#endif /* !JOS_INC_SIGNAL_H */