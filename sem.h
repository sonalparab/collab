#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>

#define KEY 832
#define COLLABKEY 835

int semid;
union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array; 
    struct seminfo *__buf;
};


#ifndef SEM
#define SEM
int create_sem(int,int);

void remove_sem(int);

int view_sem(int);

void decrement_sem(int);

void increment_sem(int);
#endif
