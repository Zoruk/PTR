#include <rtdk.h>
#include <native/task.h>
#include <native/timer.h>
#include <time.h>
#include <stdlib.h>
#include <sys/mman.h>

// Structure pour passer des donnee a la tache Xenomai
typedef struct task_data {
    RTIME  period;
    unsigned long nb_loop;    
} task_data_t;

// Fonction executee en tant que tache xenomai
void do_some_loop(void *cookie) {
    task_data_t *data = (task_data_t *)cookie;
    RTIME ts;
    RTIME last_ts;

    last_ts = rt_timer_read();
    rt_task_set_periodic(rt_task_self(), TM_NOW, data->period);
    unsigned long i = 0;

    // Effectue la tache periodique n fois
    while (0 == rt_task_wait_period(NULL)) {
        ts = rt_timer_read();
 
        rt_printf("%ld\n", ts - last_ts);
        last_ts = ts;
        if (++i == data->nb_loop) return;
       
    }
}

int main(int argc, char *argv[]){
    // Desactive le SWAP pour lapp
    mlockall(MCL_CURRENT | MCL_FUTURE);
    // Initialise rt_printf
    rt_print_auto_init(1);
    
    task_data_t t_data;
    int error;

    RT_TASK task;

    if (argc < 3){
        rt_printf ("Require argument : nb_loop, sleep time in ns\n");
        return EXIT_FAILURE;
    }

    // Recupere les arguments
    t_data.period = atol(argv[2]);
    t_data.nb_loop = atol(argv[1]);
    
    // Cree et demare la tache Xenomai
    if ((error = rt_task_spawn(&task, "First timer task", 0, 99, T_JOINABLE, do_some_loop, &t_data)) != 0) {
        rt_fprintf(stderr, "rror: cannot create the task (%s)\n", strerror(-error));
        return EXIT_FAILURE;
    }

    // Attend la fin de la tache
    if (rt_task_join(&task)) {
        rt_fprintf(stderr, "Can not join task");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}	
