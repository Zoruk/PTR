
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


void(*watchdog_suspend_function)(void)=NULL; /**< Fonction appelée lors d'une surcharge */



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

  return 1;
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

  return 1;
}

