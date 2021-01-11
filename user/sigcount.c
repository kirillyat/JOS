#include <inc/lib.h>
#include <inc/signal.h>

void
handler(int val)
{
    cprintf("Receiving > %d\n", val);
    int i;
    for (i = 0; i <= val; ++i)
    {
        cprintf("%d\n", i);
    }
    return;
}


void
umain(int argc, char **argv)
{
    struct sigaction sa = {handler};
    sys_sigaction(SIG_test, &sa);
    
    while (1) {}
}