#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

#define CLOCK_TYPE CLOCK_REALTIME

// Structure pour passer des donnee au thread
typedef struct task_data {
    struct timespec sleep_time;
    unsigned long nb_loop;    
} task_data_t;

// Fonction executee par le thread
void *do_some_loop(void *cookie) {
    task_data_t *data = (task_data_t *)cookie;
    struct timespec ts;
    struct timespec last_ts;
    long long time;
    long long last_time;
    unsigned long i;

    // Effectue la tache toutes les temps donne
    clock_gettime(CLOCK_TYPE, &last_ts);
    last_time = last_ts.tv_sec * 1000000000 + last_ts.tv_nsec;
    for (i = 0; i < data->nb_loop; ++i) {
        nanosleep(&(data->sleep_time), NULL);
        clock_gettime(CLOCK_TYPE, &ts);

        time = ts.tv_sec * 1000000000 + ts.tv_nsec;
 
        printf("%lld\n", time - last_time);
        last_ts = ts;
        last_time = time;

    }
    return NULL;
}

int main(int argc, char *argv[]){
    unsigned long usec_sleep;
    task_data_t t_data;
    pthread_t thread;

    if (argc < 3){
        printf ("Require argument : nb_loop, sleep time in ns\n");
        return EXIT_FAILURE;
    }

    // Recupere les paramettres
    usec_sleep = atol(argv[2]);
    t_data.sleep_time.tv_sec = usec_sleep / 1000000000ul;
    t_data.sleep_time.tv_nsec = usec_sleep % 1000000000ul;

    t_data.nb_loop = atol(argv[1]);
    
    // Cree le thread
    if (pthread_create(&thread, NULL, do_some_loop, &t_data)) {
        fprintf(stderr, "Faild to create thread");
        return EXIT_FAILURE;
    }

    // Attend la fin de l'execution
    if (pthread_join(thread, NULL)) {
        fprintf(stderr, "Can not join thread");
        return EXIT_FAILURE;
    } 
    return EXIT_SUCCESS;
}
