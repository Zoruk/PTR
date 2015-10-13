#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)

void timer_handler (int signum)
{
    static int count = 0;
    printf ("timer expired %d times\n", ++count);
}

static void handler(int sig, siginfo_t *si, void *uc) {
    /* Note: calling printf() from a signal handler is not
       strictly correct, since printf() is not async-signal-safe;
       see signal(7) */

    printf("Caught signal %d\n", sig);
    //print_siginfo(si);
    //signal(sig, SIG_IGN);
}

int main (int argc, char** argv)
{

    const unsigned long int nb_mesure = atoll(argv[1]);
    const unsigned long int interval = atoll(argv[2]);

    timer_t timerid;
    struct sigevent sev;
    struct itimerspec its;
    long long freq_nanosecs;
    sigset_t mask;
    struct sigaction sa;
   
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <sleep-secs> <freq-nanosecs>\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }
    /* Install timer_handler as the signal handler for SIGVTALRM. */
    printf("Establishing handler for signal %d\n", SIG);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIG, &sa, NULL) == -1)
        errExit("sigaction"); 

    /* Configure the timer to expire after 250 msec... */

    /* ... and every 250 msec after that. */

    /* Start a virtual timer. It counts down whenever this process is
      executing. */

    /* Do busy work. */
    while (1);
}

