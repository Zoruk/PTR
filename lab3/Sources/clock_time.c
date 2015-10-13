#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NB_MESURES 30

int main(int argc, char *argv[]){

   struct timespec ts;
   int i;

   for(i = 0; i < NB_MESURES; i ++){
	  // CLOCK_REALTIME permet de récupérer le temps actuel
      clock_gettime(CLOCK_REALTIME, &ts);
      printf("%2d : %ld.%09ld\n", i, ts.tv_sec, ts.tv_nsec);
   }
   
   return EXIT_SUCCESS;
}
