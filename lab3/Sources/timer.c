#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

static int nbMesure;

void handler_signal (int signum)
{
    static char first = 0;
    struct timespec ts;
    static struct timespec last_ts;

    //vérifie si c'est le premier signal
    if(first == 1){

        clock_gettime(CLOCK_REALTIME, &ts);  
        double time = ts.tv_sec + ts.tv_nsec / 1000000000.0; 
        double last_time = last_ts.tv_sec + last_ts.tv_nsec / 1000000000.0;  
 
        printf("%f\n", time-last_time);
        last_ts = ts;

        nbMesure--;
        if(nbMesure <= 0){
            exit(EXIT_SUCCESS);    
        }
    }else{
        first = 1;
        clock_gettime(CLOCK_REALTIME, &last_ts);    
    }
    
}

int main(int argc, char *argv[]){

    // vérifie le nombre d'argument
    if(argc < 3){
        printf ("Need 2 arguments: nb_mesure & time_usec\n");
        return EXIT_FAILURE;    
    }

    // récupere le nombre de mesure et le temps intervalles
    nbMesure = atoi(argv[1]);
    unsigned long usec = atoi(argv[2]);

    timer_t timer;
    struct sigevent event;
    struct itimerspec spec;
    unsigned long nsec;

    // Configurer le timer
    signal(SIGRTMIN, handler_signal);
    event.sigev_notify = SIGEV_SIGNAL;
    event.sigev_signo = SIGRTMIN;
    nsec = usec*1000; // en nanosec
    spec.it_interval.tv_sec = nsec / 1000000000;
    spec.it_interval.tv_nsec = nsec % 1000000000;
    spec.it_value = spec.it_interval;

    // Allouer le timer
    timer_create(CLOCK_REALTIME, & event, & timer);

    // Programmer le timer
    timer_settime(timer, 0, &spec, NULL);
   
    while(1);
    return EXIT_SUCCESS;
}

