
/**
* \file intro_watchdog.c
* \brief Application pour l'introduction aux concept de tâche.
* \author Y. Thoma
* \version 0.2
* \date 13 octobre 2015
*
* Ce laboratoire permet de tester les mécanismes de base
* de l'exécutif Xenomai pour la création et la gestion
* de tâches temps-réel. Il met également en place un
* watchdog qui pourra être exploité lors de futurs
* laboratoires.
*
*/

#include <sys/mman.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include <rtdk.h>
#include <native/task.h>
#include <native/timer.h>

#include "general.h"

#include "busycpu.h"

#include "watchdog.h"


#define PERIOD_TASK1       1000        /**< Période de la tâche 1 */
#define PERIOD_TASK2       2000        /**< Période de la tâche 2 */
#define PERIOD_TASK3       4000        /**< Période de la tâche 3 */

#define CPU_TASK1          50          /**< Temps de traitement de la tâche 1 */
#define CPU_TASK2          100         /**< Temps de traitement de la tâche 2 */
#define CPU_TASK3          100         /**< Temps de traitement de la tâche 3 */

#define CPU_TASK1_INC      10          /**< Temps de traitement ajouté à chaques iteration de la tâche 1 */
#define CPU_TASK2_INC      20          /**< Temps de traitement ajouté à chaques iteration de la tâche 2 */
#define CPU_TASK3_INC      100         /**< Temps de traitement ajouté à chaques iteration de la tâche 3 */

#define PRIO_TASK1         10          /**< Priorité de la tâche 1=faible, 99=forte */
#define PRIO_TASK2         15          /**< Priorité de la tâche 1=faible, 99=forte */
#define PRIO_TASK3         20          /**< Priorité de la tâche 1=faible, 99=forte */

RT_TASK myTask1;  /**< Descripteur de la tâche périodique 1 */
RT_TASK myTask2;  /**< Descripteur de la tâche périodique 2 */
RT_TASK myTask3;  /**< Descripteur de la tâche périodique 3 */
                      
int installed_task1=0; /**< Indique si la tâche 1 est chargée dans le module */
int installed_task2=0; /**< Indique si la tâche 2 est chargée dans le module */
int installed_task3=0; /**< Indique si la tâche 3 est chargée dans le module */

int err; /**< stockage du code d'erreur */

int process_interrupt=1; /**< Indique si l'interruption doit être traitée ou non */

void cleanup_objects(void);

void suspendre(void)
{
  rt_printf("Appel Suspendre()\n");
  if (installed_task1) {
    rt_printf("Supprime la tache 1\n");
    rt_task_delete(&myTask1);
    installed_task1=0;
  }
  if (installed_task2) {
    rt_printf("Supprime la tache 2\n");
    rt_task_delete(&myTask2);
    installed_task2=0;
  }
  if (installed_task3) {
    rt_printf("Supprime la tache 3\n");
    rt_task_delete(&myTask3);
    installed_task3=0;
  }
  return;
}



/** \brief Tâche périodique
*
* La fonction  busy_cpu(..) permet de consommer (inutilement) du temps CPU;
* utile pour simuler des charges!
* La fonction rt_printf() permet d'afficher quelque chose sur la console.
* Elle s'utilise comme la fonction rt_printf()
* \param cookie Non utilisé
*/
void periodicTask1(void *cookie) {

  /* Configuration de la tâche périodique */
  if (TIMER_PERIODIC){
    err = rt_task_set_periodic(&myTask1, rt_timer_read() + PERIOD_TASK1, PERIOD_TASK1); /* mode périodique (en ticks) */
    if (err != 0) {
      printf("task set periodic failed: %s\n", strerror(-err));
      return;
    }
  }
  else{
    RTIME period = ((RTIME)PERIOD_TASK1)*((RTIME)MS);
    err = rt_task_set_periodic(&myTask1, rt_timer_read() + period, period); /* mode apériodique, ONE_SHOT */
    if (err != 0) {
      printf("task set periodic failed: %s\n", strerror(-err));
      return;
    }
  }

  rt_printf("Demarrage de la tache periodique 1\n", TM_NOW);

  int i = 0;
  while (1) {
    
    /* simulation traitement */
    rt_printf("Tache periodique 1: debut d'execution\n");
    busy_cpu(CPU_TASK1 + CPU_TASK1_INC * i++);
    rt_printf("Tache periodique 1: fin d'execution\n");

    rt_task_wait_period(NULL);
  } 

}



/** \brief Tâche périodique
*
* La fonction  busy_cpu(..) permet de consommer (inutilement) du temps CPU;
* utile pour simuler des charges!
* La fonction rt_printf() permet d'afficher quelque chose sur la console.
* Elle s'utilise comme la fonction rt_printf()
* \param cookie Non utilisé
*/
void periodicTask2(void *cookie) {

  /* Configuration de la tâche périodique */
  if (TIMER_PERIODIC){
    err = rt_task_set_periodic(&myTask2, rt_timer_read() + PERIOD_TASK2, PERIOD_TASK2); /* mode périodique (en ticks) */
    if (err != 0) {
      printf("task set periodic failed: %s\n", strerror(-err));
      return;
    }
  }
  else{
    RTIME period = ((RTIME)PERIOD_TASK2)*((RTIME)MS);
    err = rt_task_set_periodic(&myTask2, rt_timer_read() + period, period); /* mode apériodique, ONE_SHOT */
    if (err != 0) {
      printf("task set periodic failed: %s\n", strerror(-err));
      return;
    }
  }

  rt_printf("Demarrage de la tache periodique 2\n", TM_NOW);
  int i = 0;
  while (1) {
    
    /* simulation traitement */
    rt_printf("Tache periodique 2: debut d'execution\n");
    busy_cpu(CPU_TASK2 + CPU_TASK2_INC * i++);
    rt_printf("Tache periodique 2: fin d'execution\n");

    rt_task_wait_period(NULL);
  } 

}

/** \brief Tâche périodique
*
* La fonction  busy_cpu(..) permet de consommer (inutilement) du temps CPU;
* utile pour simuler des charges!
* La fonction rt_printf() permet d'afficher quelque chose sur la console.
* Elle s'utilise comme la fonction rt_printf()
* \param cookie Non utilisé
*/
void periodicTask3(void *cookie) {

  /* Configuration de la tâche périodique */
  if (TIMER_PERIODIC){
    err = rt_task_set_periodic(&myTask3, rt_timer_read() + PERIOD_TASK3, PERIOD_TASK3); /* mode périodique (en ticks) */
    if (err != 0) {
      printf("task set periodic failed: %s\n", strerror(-err));
      return;
    }
  }
  else{
    RTIME period = ((RTIME)PERIOD_TASK3)*((RTIME)MS);
    err = rt_task_set_periodic(&myTask3, rt_timer_read() + period, period); /* mode apériodique, ONE_SHOT */
    if (err != 0) {
      printf("task set periodic failed: %s\n", strerror(-err));
      return;
    }
  }

  rt_printf("Demarrage de la tache periodique 3\n", TM_NOW);

  int i = 0;
  while (1) {
    
    /* simulation traitement */
    rt_printf("Tache periodique 3: debut d'execution\n");
    busy_cpu(CPU_TASK3 + CPU_TASK3_INC * i++);
    rt_printf("Tache periodique 3: fin d'execution\n");

    rt_task_wait_period(NULL);
  } 

}



void catch_signal(int sig)
{
  cleanup_objects();
}

/** \brief Routine appelée au chargement du module
*
* Cette routine est appelé lors du chargement du module dans l'espace noyau.
* C'est le point d'entrée de notre application temps-réel.
*/
int main(int argc, char* argv[]) {

  printf("Initialisation\n");

  signal(SIGTERM, catch_signal);
  signal(SIGINT, catch_signal);

  /* Avoids memory swapping for this program */
  mlockall(MCL_CURRENT|MCL_FUTURE);

  /* Init the rt_print buffer and task */
  rt_print_auto_init(1);

  /* Initialisation du timer */
  if (TIMER_PERIODIC){
    printf("Test if timer periodic: entree\n");
    err = rt_timer_set_mode(MS); // mode périodique, un tick vaut 1 ms
    printf("Test if timer periodic: sortie\n");
  }
  else {
    printf("Test if timer aperiodic: entree\n");

    printf("valeur TM_ONESHOT: %d \n", TM_ONESHOT);

    err = rt_timer_set_mode(TM_ONESHOT); // mode apériodique
	
    printf("Test if timer aperiodic: sortie\n");
  }

  if (err != 0) {
    printf("Error timer: %s\n", strerror(-err));
    return -1;
  }

  if (start_watchdog(suspendre))
    goto fail;


  /* Création de la tâche périodique 1 */
  err =  rt_task_spawn (&myTask1, "myTask1", STACK_SIZE, PRIO_TASK1, 0, periodicTask1, 0);
  if (err != 0) {
    printf("task 1 creation failed: %s\n", strerror(-err));
    goto fail;
  }
  else
    installed_task1=1;


  
  /* Création de la tâche périodique 2 */
  err =  rt_task_spawn (&myTask2, "myTask2", STACK_SIZE, PRIO_TASK2, 0, periodicTask2, 0);
  if (err != 0) {
    printf("task 2 creation failed: %s\n", strerror(-err));
    goto fail;
  }
  else
    installed_task2=1;
    


  /* Création de la tâche périodique 3 */
  err =  rt_task_spawn (&myTask3, "myTask3", STACK_SIZE, PRIO_TASK3, 0, periodicTask3, 0);
  if (err != 0) {
    printf("task 3 creation failed: %s\n", strerror(-err));
    goto fail;
  }
  else
    installed_task3=1;


  pause();

  return 0;
  
  fail:
   cleanup_objects();
   return -1;
}

/** \brief Routine appelée lors de la destruction du module
*
* Routine de nettoyage des objets alloués en mémoire noyau.
* Cette routine est appelée lorsque le module est ôté du noyau (rmmod).
*/
void cleanup_objects (void) {

  printf("intro_watchdog: destruction des objets\n");

  /* Destruction du watchdog */
  end_watchdog();
  
  /* Destruction des objets dans l'ordre inverse de leur création
   * afin d'éviter tout problème de dépendance entre les objets */
  if (installed_task3)
	  rt_task_delete(&myTask3);
  if (installed_task2)
	  rt_task_delete(&myTask2);
  if (installed_task1)
	  rt_task_delete(&myTask1);
  
  printf("intro_watchdog : bye bye!\n");
}

