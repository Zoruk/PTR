#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NB_MESURES 30

int main(int argc, char *argv[]){

   struct timespec ts[NB_MESURES];
   int i;

   for(i = 0; i < NB_MESURES; i ++){
	  // CLOCK_REALTIME permet de récupérer le temps actuel
      clock_gettime(CLOCK_REALTIME, ts+i);
   }
   
   for(i = 0; i < NB_MESURES; i ++){
      printf("%2d : %ld.%09ld\n", i, ts[i].tv_sec, ts[i].tv_nsec);
   }
   
   return EXIT_SUCCESS;
}
