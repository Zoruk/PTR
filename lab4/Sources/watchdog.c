
/**
* \file watchdog.c
* \brief Fonctions permettant la mise en place d'un watchdog.
* \author Y. Thoma & D. Molla
* \version 0.1
* \date 29 septembre 2010
*
* Le watchdog proposé est implémenté à l'aide de 2 tâches responsables
* de détecter une surcharge du processeur. Si tel est le cas, une fonction
* fournie par le développeur est appelée. Elle devrait suspendre ou détruire
* les tâches courantes, afin de redonner la main à l'OS.
*
*/

#include <rtdk.h>
#include <native/task.h>
#include <native/intr.h>
#include <native/alarm.h>
#include <native/timer.h>

#include "watchdog.h"
#include "general.h"

// 1ms
#define CANARI_PERIOD 1000000
// 3 sec
#define WATCHDOG_PERIOD (CANARI_PERIOD * 3000ll)

typedef unsigned long CPT_TYPE;

static void(*watchdog_suspend_function)(void)=NULL; /**< Fonction appelée lors d'une surcharge */
static char stop_watchdog = 0;
static CPT_TYPE canari_cpt = 0;
static RT_TASK task_canari;
static RT_TASK task_watchdog;

/** \brief Tache du canary
* 
*/
static void task_canari_fct(void *data) {
    RTIME time = CANARI_PERIOD;

    rt_task_set_periodic(rt_task_self(), TM_NOW, time);

    // Effectue dit cui cui souvant
    while (0 == rt_task_wait_period(NULL) && stop_watchdog == 0) {
         canari_cpt += 1; // Cui cui
    }
}

/**
*
*/
static void task_watchdog_fct(void *data) {
    RTIME max_freez = WATCHDOG_PERIOD;
    
    rt_task_set_periodic(rt_task_self(), TM_NOW, max_freez);
    CPT_TYPE last_canari_cpt = -1;

    // Vérifie que le canari dit cui cui suffisament souvant
    while (0 == rt_task_wait_period(NULL) && stop_watchdog == 0) {
        rt_printf("Watchdod: check canarycpt = %ul, last = %ul\n", canari_cpt, last_canari_cpt);
        if (last_canari_cpt == canari_cpt) {
            rt_printf("Watchdog : Arret du programme\n");
            if (watchdog_suspend_function) 
                watchdog_suspend_function();
            //stop_watchdog = 1;
        }
        last_canari_cpt = canari_cpt;
    }
}

/** \brief Démarre le watchdog
*
* Le watchdog lance deux tâches. Une tâche de faible priorité qui incrémente
* un compteur, et une de priorité maximale mais de période moindre qui
* vérifie que le compteur s'est incrémenté. Si tel n'est pas le cas,
* la fonction passée en paramètre est appelée. Elle devrait contenir du code
* responsable de suspendre ou supprimer les tâches courantes.
* \param suspend_func Fonction appelée par le watchdog en cas de surcharge
*/
int start_watchdog(void(* suspend_func)(void))
{
    watchdog_suspend_function=suspend_func;
    stop_watchdog = 0;
    int error;
    
    if ((error = rt_task_spawn(&task_canari, "Canari task", 0, 0, 0, task_canari_fct, NULL)) != 0) {
        rt_printf("error: cannot create the canari task (%s)\n", strerror(-error));
        return -1;
    }

    if ((error = rt_task_spawn(&task_watchdog, "Watchdog task", 0, 99, 0, task_watchdog_fct, NULL)) != 0) {
        rt_printf("error: cannot create the watchdog task (%s)\n", strerror(-error));
        return -1;
    }
    return 0;
}


/** \brief Termine les deux tâches du watchdog
*
* Cette fonction doit être appelée par la fonction de cleanup du module
* afin de libérer les ressources allouées par le watchdog.
* En l'occurence, les deux tâches du watchdog sont supprimées.
* Elle peut aussi être appelée à n'importe quel instant pour supprimer
* la surveillance du watchdog.
*/
int end_watchdog(void)
{
    stop_watchdog = 1;
    int result;
    if (0 != (result = rt_task_delete(&task_canari))) {
        rt_fprintf(stderr, "Impossible de delete la tache canari err : %s\n", strerror(-result));
        return -1;
    }
    if (0 != (result = rt_task_delete(&task_watchdog))) {
        rt_fprintf(stderr, "Impossible de delete la tache canari err : %s\n", strerror(-result));
        return -1;
    }
    return 0;
}

