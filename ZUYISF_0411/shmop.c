#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define KEY 2022

void main()
{
    int sharedMemoryId = shmget(KEY, 0, 0);

    char *segm = shmat(sharedMemoryId, NULL, SHM_RND);
    strcpy(segm, "Egy uj uzenet erkezett");

    printf("A kozos memoria tartalma: %s\n", segm);

    shmdt(segm);
}