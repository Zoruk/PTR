
/**
* \file busycpu.c
* \brief Fonctions permettant la simulation de consommation de temps CPU.
* \author Y. Thoma
* \version 0.2
* \date 14 novembre 2014
*
* La fonction busy_cpu() permet de simuler du temps de traitement à l'aide
* d'une boucle. Cette version exploite la librairie rtlog pour permettre
* de visualiser l'évolution du système.
*
*/

#include <native/task.h>
#include <native/intr.h>
#include <native/alarm.h>
#include <native/timer.h>

#include "busycpu.h"

#define MS                 1000000     /**< 1 ms exprimé en ns */

/** \brief Fonction permettant de simuler de l'utilisation de temps CPU
*
* Cette fonction peut être appelée par plusieurs tâches de manière
* concurrente. Chaque tâche consommera ensuite le temps CPU demandé.
*
* \param ms Nombre de millisecondes à consommer
*/
void busy_cpu(int ms) {

  volatile int fake=0;
  int i;

  RT_TASK_INFO info1,info2;
  rt_task_inquire(rt_task_self(),&info1);
  RTIME total=((RTIME)ms)*((RTIME)(1000000));

  do {
    for (i=0;i<10;i++){
        fake++;
        fake++;
        fake++;
        fake-=3;
    }
    rt_task_inquire(rt_task_self(),&info2);
  } while (info2.exectime-info1.exectime<total);

}

