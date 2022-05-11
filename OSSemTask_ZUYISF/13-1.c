#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define SHMKEY 67L
int main()
{
	int shmid;		/* osztott memoria azonosito */
	key_t key;		/* kulcs a shmem-hez */
	int size=512;		/* osztott szegmens merete byte-ban */
	int shmflg;		/* flag, ami a jellemzokhoz kell */
	
	struct vmi {
		int  hossz;
		char szoveg[512-sizeof(int)]; /* az egesz merete 512, ezert a "hossz" valtozot levonjuk a tombbol */
	} *segm;		/* Ezt a strukturat kepezzuk a szegmensre */
	
	key = SHMKEY;

	shmflg = 0;	/* Nincs IPC_CREAT, feltetelezzuk, hogy az shmcreate.c keszitett osztott memoria szegmenst */

	if ((shmid=shmget(key, size, shmflg)) < 0) {
	    perror("Az shmget system-call sikertelen!\n");
	    exit(-1);
	}

	/* Attach - rakapcsolodunk*/

	shmflg = 00666 | SHM_RND;	/* RND az igazitashoz */
	segm = (struct vmi *)shmat(shmid, NULL, shmflg);
				/* Itt a NULL azt jelenti, hogy az OS-re bizom, milyen cimtartomanyt hasznaljon. */
	if (segm == (void *)-1) {
		perror("Sikertelen attach!\n");
		exit(-1);
	}

	/* Sikeres attach utan a "segm" cimen ott az osztott memoria. Ha van benne valami, kiiratom, utana billentyuzetrol kerek uj beteendo szoveget */

	if (strlen(segm->szoveg) > 0) 
		printf("\n Regi szoveg: %s (%d hosszon)", segm->szoveg,segm->hossz);
	int rtn = shmctl(shmid, IPC_RMID, NULL);
		printf("Szegmens torolve. Hibakod: %d\n", rtn);
		return 0;
		}