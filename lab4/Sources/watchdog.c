
/**
* \file watchdog.c
* \brief Fonctions permettant la mise en place d'un watchdog.
* \author Y. Thoma, D. Molla, L. Haas
* \version 0.2
* \date 01 septembre 2015
*
* Le watchdog proposé est implémenté à l'aide de 2 tâches responsables
* de détecter une surcharge du processeur. Si tel est le cas, une fonction
* fournie par le développeur est appelée. Elle devrait suspendre ou détruire
* les tâches courantes, afin de redonner la main à l'OS.
*
* L'implementation est faites de la manière suivante :
*     - Une tache canari de priorité très faible incremante une variable global tout les temps CANARY
*     - Une tache watchdog de priorité maximale péridique executée avec un temps > CANARY vérifie que 
*           la variable global incremanté par la tache ci-dessus ai changée si elle est identiique appel
*           la fonction passée par le dévloppeur affin d'arreter les tache provoquant une surcharge CPU.
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
// 2 sec
#define WATCHDOG_PERIOD (CANARI_PERIOD * 2000ll)

typedef unsigned long CPT_TYPE;

static void(*watchdog_suspend_function)(void)=NULL; /**< Fonction appelée lors d'une surcharge */
static char stop_watchdog = 0;
static CPT_TYPE canari_cpt = 0;
static RT_TASK task_canari;
static RT_TASK task_watchdog;

/** \brief Tache du canary
*      Tache du canary incremante une variable global tout les temps fix 
*/
static void task_canari_fct(void *data) {
    RTIME time = CANARI_PERIOD;

    rt_task_set_periodic(rt_task_self(), TM_NOW, time);

    // Effectue dit cui cui souvant
    while (stop_watchdog == 0) {
         canari_cpt += 1; // Cui cui
         rt_task_wait_period(NULL);
    }
}

/**
*  Tache watchdog effectue la varification que le canary sois bien incremanté 
*    affin de tetecter une éventuelle surcharge
*/
static void task_watchdog_fct(void *data) {
    RTIME max_freez = WATCHDOG_PERIOD;
    
    rt_task_set_periodic(rt_task_self(), TM_NOW, max_freez);
    CPT_TYPE last_canari_cpt = -1;

    // Vérifie que le canari dit cui cui suffisament souvant
    while (0 == rt_task_wait_period(NULL) && stop_watchdog == 0) {
        // Affiche l'iteration du watchdog pour du débug
        rt_printf("Watchdod: check canarycpt = %u, last = %u\n", canari_cpt, last_canari_cpt);
        if (last_canari_cpt == canari_cpt) {
            rt_printf("Watchdog : Surcharge détectée appel de la fonction d'arret\n");
            if (watchdog_suspend_function) 
                watchdog_suspend_function();
            else {
                // What to do here ? Maybe call exit(-1) ?
                // Depend on implementation
                // exit(-1);
            }
            //stop_watchdog = 1; // On pourrais aussi arreter le watchdog
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
 
    // Start canary task prioriry 0   
    if ((error = rt_task_spawn(&task_canari, "Canari task", 0, 0, 0, task_canari_fct, NULL)) != 0) {
        rt_printf("error: cannot create the canari task (%s)\n", strerror(-error));
        return -1;
    }

    // Start watchdog task priority 99
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

