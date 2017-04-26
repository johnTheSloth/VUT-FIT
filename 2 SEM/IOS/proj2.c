/* IOS Projekt c.2
 * Autor: Grossmann Jan
 * Datum: 1. kvetna 2016
 * Popis: Problem synchronizace procesu
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>      // key_t, sem_t, pid_t
#include <sys/shm.h>        // shmat(), IPC_RMID
#include <errno.h>          // errno, ECHILD
#include <semaphore.h>      // sem_open(), sem_destroy(), sem_wait()..
#include <fcntl.h>          //O_CREAT, O_EXEC
#include <unistd.h>			//fork
#include <sys/types.h> 		//waitpid
#include <sys/wait.h>		//waitpid
#include <sys/mman.h>
#include <time.h>    		// time()

int main (int argc, char **argv){
	if(argc != 5) {	//Test na pocet argumentu, kdyz neprojde program se ukonce
		printf("Nespravny pocet argumentu\n");
		exit(2);
	}	

	FILE *fp = fopen("proj2.out", "w"); // Ottevreni souboru pro zapis vystupu
	if (NULL == fp) {	//Pokud se otevreni nezdarilo, ukonci se
		perror("fopen");
		exit(2);
	}
	setbuf(fp, NULL);	//Nastaveni potrebne pro spravny zapis proj2.out
    int i;	//Pomocny loop index
    sem_t *semLoad;	//Semafor zacatek nastupu
    sem_t *semLoadEnd;	//Semafor konec nastupu
    sem_t *semUnload;	//Semafor zacatek vystupu
    sem_t *semUnloadEnd;	//Semafor konec vystupu
    sem_t *semFinished;	//Semafor konec vsech jizd
    pid_t pid; //Forkovaci PID
    int P;		//Pocet pasazeru
    int value;	//Pocet mist v car

	//Serie testu na spravnost vstupu
	char *ptr;	//Pointer pro test strtol
	P = strtol(argv[1], &ptr, 10);
	if (*ptr != '\0') {
		printf("Nespravny argument\n");
        exit(2);
	}
	
	value = strtol(argv[2], &ptr, 10);
	if (*ptr != '\0') {
		printf("Nespravny argument\n");
        exit(2);
	}
	
	long PT = 0;	//Doba po kterou se generuje proces pasazer
	PT = strtol(argv[3], &ptr, 10);
	if (*ptr != '\0') {
		printf("Nespravny argument\n");
        exit(2);
	}
	
	long RT = 0;	//Doba cesty voziku po trati
	RT = strtol(argv[4], &ptr, 10);
	if (*ptr != '\0') {
		printf("Nespravny argument\n");
        exit(2);
	}
	
	if(PT > 5000 && RT > 5000 && PT < 0 && RT < 0 && P <= 0 && value <= 0 && P%value != 0){ //Dalsi test na spravnost zadanych argumentu
		printf("Nespravny argument\n");
        exit(2);
	}


	/******************   SHARED MEMORY    ****************/
	int *finished;	//Testovaci promena 
    finished = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *finished = 0;
    
    int *A; //operace
    A = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *A = 1;
    
    int *boarded; //Testovaci promena ktera urci jestli vsichni pasazeri projeli trati
    boarded = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *boarded = 0;
    
    
    /******************   SEMAFORY    ****************/
    semLoad = sem_open("/xgrossLoad", O_CREAT | O_EXCL, 0644, 0);	//Inicializace semaforu pro operaci Load
    sem_unlink("/xgrossLoad");   	//Unlink aby semafor nebezel donekonecna   

    semLoadEnd = sem_open("/xgrossLoadEnd", O_CREAT | O_EXCL, 0644, 0); 
    sem_unlink("/xgrossLoadEnd");      

    semUnload = sem_open("/xgrossUnload", O_CREAT | O_EXCL, 0644, 0); 
    sem_unlink("/xgrossUnload");      

    semUnloadEnd = sem_open("/xgrossUnloadEnd", O_CREAT | O_EXCL, 0644, 0); 
    sem_unlink("/xgrossUnloadEnd");      

    
    semFinished = sem_open("/xgrossFinished", O_CREAT | O_EXCL, 0644, 0); 
    sem_unlink("/xgrossFinished");      

    /***************************************************/
 	time_t t;
	srand((unsigned) time(&t));  //Inicializace random gen

	pid= fork();	//Fork potrebny pro vytvoreni procesu Car
	if (pid < 0){
        exit(2);
	}else if (pid == 0){	//Proces Car
		fprintf(fp, "%d:\tC 1:\tstarted\n",*A);
		while(*boarded != P){	//Dokud se neprepravili vsichni pasazeri
			for(int j = 0; j < value; j++){	//Signalizace pro x procesu
				if(j == 0) {
					*A += 1;					
					fprintf(fp, "%d:\tC 1:\tload\n",*A);
				}
				sem_post(semLoad);
				sem_wait(semLoadEnd);
			}
			*A += 1;	//Zvzseni counteru poctu operaci
			fprintf(fp, "%d:\tC 1:\trun\n",*A);
			
			int rnumber;
			rnumber=rand()% (RT*1000 +1) ;	//Urceni casova prodlevy
			usleep(rnumber);	//Aktualni cekani
			for(int l = 0; l < value; l++){
				if(l == 0) {
					*A += 1;					
					fprintf(fp, "%d:\tC 1:\tunload\n",*A);
				}
				sem_post(semUnload);	//Signalizace pro sync. unload procesu
				sem_wait(semUnloadEnd);
			}

		}
		for(int q = 0; q < P; q++){
			sem_post(semFinished);	//Zachytny bod pro konec procesu pasazeru,, 'finished'
		}
		
		*A += 1;
		fprintf(fp, "%d:\tC 1:\tfinished\n",*A); //Vypsani a ukonceni procesu car
		exit(0);
	}

	pid= fork();	//Fork potrebny pro vytvoreni pomocneho procesu
	if (pid < 0){
        exit(2);
	}
	
	/******************   POMOCNY PROCES    ****************/
	else if (pid == 0){
		for (i = 1; i <= P; i++){ //Forknuti podle poctu pasazeru
			int rnumber;	//rnumber je nahodne cislo po kterou se generuje pasazer
			rnumber=rand() % (PT * 1000 +1);
			usleep(rnumber);
		    pid = fork();
		    if (pid < 0){	//Test na error
       	 		exit(2);
       	 	}
		    else if (pid == 0)
		        break;	//Priprava pro pozdejsi praci
		}

		/******************   PASAZER PROCES    ****************/
		if (pid == 0){
			*A += 1;
			fprintf(fp, "%d:\tP %d:\tstarted\n",*A,i);
			sem_wait(semLoad);	//Ceka se dokud car nepovoli semLoad
			*boarded += 1;	//Inkrementuje hodnotu prepravenych lidi
			*A += 1;
			fprintf(fp, "%d:\tP %d:\tboard\n",*A,i);
			*A += 1;
			if(i % value != 0) {
				fprintf(fp, "%d:\tP %d:\tboard order %d\n",*A,i, i % value);
			}else {
				fprintf(fp, "%d:\tP %d:\tboard last\n",*A,i);
			}
			sem_post(semLoadEnd);

			sem_wait(semUnload);	//Ceka dokud car nezaveli unload
			*A += 1;
			fprintf(fp, "%d:\tP %d:\tunboard\n",*A,i);
			*A += 1;
			if(i % value != 0) {
				fprintf(fp, "%d:\tP %d:\tunboard order %d\n",*A,i, i % value);
			}else {
				fprintf(fp, "%d:\tP %d:\tunboard last\n",*A,i);
			}

			sem_post(semUnloadEnd);
			sem_wait(semFinished);	//Ceka dokud se nepovoli semFinished, pote se ukonci
			*A += 1;
			fprintf(fp, "%d:\tP %d:\tfinished\n",*A,i);
			exit(0);
			  
		    }
		}
		

		/******************   HLAVNI PROCES    ****************/
		else{
		    while ((pid = waitpid(-1, NULL, 0))){	//Pocka na ukonceni vsech childu
		        if (errno == ECHILD)
		            break;
		    }

		    // Uklid sharedm memory
		    shmdt(finished);
		    shmdt(A);
		    shmdt(boarded);

		    //Uklid semaforu
		    sem_destroy(semLoad);
		    sem_destroy(semLoadEnd);
		    sem_destroy(semUnload);
		    sem_destroy(semUnloadEnd);
		    sem_destroy(semFinished);
		    
		    fclose(fp);
		    exit(0);
		}
    
}
