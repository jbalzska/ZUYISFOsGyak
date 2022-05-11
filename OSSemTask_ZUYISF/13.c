/*	shmcreate.c	*/
/*	SHMKEY  kulccsal kreal/azonosit osztott memoria szegmenst. */
/*	Azonositoja: shmid, amit kiiratunk */

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include<unistd.h>
#define SHMKEY 67L
#define KEY 41L
#define PERM 00666

int main()
{
	int shmid;		/* osztott memoria azonosito */
	key_t key;		/* kulcs a shmem-hez */
	int size=512;	/* osztott szegmens merete byte-ban */
	int shmflg;		/* flag a jellemzokhoz */
	
	struct vmi {
		int  hossz;
		char szoveg[512-sizeof(int)]; /* az egesz merete 512, ezert a "hossz" valtozot levonjuk a tombbol */
	} *segm;
	key = SHMKEY;

	/* Megnezzuk, van-e mar SHMKEY kulcsu es "size" meretu szegmens. */
	shmflg = 0;
	if ((shmid=shmget(key, size, shmflg)) < 0) {
	   printf("Nincs meg szegmens! Keszitsuk el!\n"); /* Kiír egy üzenetet, ha nem létezik */
	   shmflg = 00666 | IPC_CREAT;
	   if ((shmid=shmget(key, size, shmflg)) < 0) {
	      perror("Az shmget() system-call sikertelen!\n"); /* Kiír még egy üzenetet */
	      exit(-1);
	   }
	} else printf("Van mar ilyen shm szegmens!\n"); /* Ha létezik */

	printf("\nAz shm szegmens azonositoja %d: \n", shmid);
	shmflg = 00666 | SHM_RND;	/* RND az igazitashoz */
	segm = (struct vmi *)shmat(shmid, NULL, shmflg);	/* Itt a NULL azt jelenti, hogy az OS-re bizom, milyen cimtartomanyt hasznaljon. */
	
	if (segm == (void *)-1) {
		perror("Sikertelen attach!\n");
		exit(-1);
	}
	
	int id; /* A szemafor azonosítója */
 
	if ((id = semget(KEY, 1, 0)) < 0) { /* Még nem létezik. */
		if ((id = semget(KEY, 1, PERM | IPC_CREAT)) < 0) {
		 	perror(" A szemafor nem nyitható meg. ");
			exit(-1);
		}
	}
	else    {
		perror(" Már létezik a szemafor. ");
		exit(0);
	}

	if (semctl(id, 0, SETVAL, 1) < 0) {
		perror(" Nem lehetett inicializálni. ");
	} else {
		puts(" Kész és inicializált a szemafor. ");
	}
	
	struct sembuf up[1]  =  {0,  1, SEM_UNDO};
	struct sembuf down[1] = {0, -1, SEM_UNDO};
	puts(" Itt fut a nem kritikus szakasz. ");

	semop(id, down, 1);   /* A belepesi szakasz */
	puts("    Itt fut a kritikus szakasz. ");
	strcpy(segm->szoveg,"valami szoveg benne");
	sleep(3);
	semop(id, up, 1);    /* Ez a kilepesi szakasz */
	puts(" Itt ismet nem kritikus szakasz fut. ");
	if (semctl(id, 0, IPC_RMID, 0) < 0) {
		perror(" Nem siker�lt t�r�lni.");
		exit(-1);
	}

	puts(" A szemafort megszuntettuk. \n");
	shmdt(segm);
	return 0;
	}
	
	
