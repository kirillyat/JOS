#include <inc/lib.h>
#include <inc/env.h>
#include <inc/signal.h>



int my_atoi(char *p) {
    int k = 0;
    while (*p) {
        k = (k << 3) + (k << 1) + (*p) - '0';
        p++;
     }
     return k;
}

void
umain(int argc, char **argv) 
{
    binaryname = "kill";
    if (argc < 2)
		printf("No arguments\n");
    else {    
        envid_t dest_env = my_atoi(argv[1]);
        int sig_no = (argc>2)? my_atoi(argv[2]):SIGKILL;
        int value = (argc>3)? my_atoi(argv[3]):0;

        sys_sigqueue(dest_env, sig_no, value);
    }

}