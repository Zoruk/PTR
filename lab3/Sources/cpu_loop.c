#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define n_sec 3

int main(int argc, char** argv){
	int debut = time(NULL);
	
	int nb_boucle = 0;
	while(time(NULL) < (debut + n_sec)){
		nb_boucle++;
	}
	printf("Nb operation par s: %d (pid:%d)\n", nb_boucle/n_sec, getpid());
	
	return 0;
}
