#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

static void SignalHandler(int);

int main(void)
{
    struct sigaction action;
    struct itimerval timer;

    memset(&action, 0, sizeof(action));

    action.sa_handler = SignalHandler;
    action.sa_flags = SA_RESTART;
    sigemptyset(&action.sa_mask);
    if(sigaction(SIGALRM, &action, NULL) < 0)
    {
        perror("sigaction error");
        exit(1);
    }

    /* set intarval timer (10ms) */
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 10000;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 10000;
    if(setitimer(ITIMER_REAL, &timer, NULL) < 0){
        perror("setitimer error");
        exit(1);
    }
}


static void SignalHandler(int signum)
{
    static unsigned long msec_cnt = 0;

    msec_cnt++;
    if(!(msec_cnt % 100)){
        printf("SignalHandler:%lu sec\n", (msec_cnt / 100));
    }
    return;
}
