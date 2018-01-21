#include "sem.h"

struct sembuf atomic_op;

int create_sem(int key, int val){
  //Create semaphore
  semid = semget(key,1,IPC_CREAT | IPC_EXCL | 0600);
  if(semid == -1)
    printf("could not create semaphore: %s\n",strerror(errno));
  else{
    union semun su;
    su.val = val;
    int v = semctl(semid,0,SETVAL,su);
    printf("semaphore created: %d\n",semid);
    printf("value set: %d\n",v);
  }
  return semid;
}

void remove_sem(int semid){

  //Remove semaphore
  //semid = semget(KEY,amt,0600);
  int v = semctl(semid,0,GETVAL);
    
  //block if semaphore val is 0
  while(v == 0)
    v = semctl(semid,0,GETVAL);
    
  //remove semaphore
  v = semctl(semid,0,IPC_RMID);
  if(v == -1)
    printf("could not remove semaphore: %s\n",strerror(errno));
  else
    printf("semaphore removed: %d \n",v);   
}

  
int view_sem(int semid){
  //Viewing
  //semid = semget(KEY,amt,0600);
  int v = semctl(semid,0,GETVAL);
  if(v == -1)
    printf("%s\n",strerror(errno));
  else
    printf("semaphore value: %d\n",v);
  return v;
}

void decrement_sem(int semid){
  //this is for the operation argument for semop
  atomic_op.sem_op = -1; //for down
  atomic_op.sem_num = 0; //index of semaphore we want to control
  atomic_op.sem_flg = SEM_UNDO; //allow OS to undo given operation
  semop(semid, &atomic_op, 1); //now performing operation on the semaphore

}

void increment_sem(int semid){
  //up or "release" the semaphore
  atomic_op.sem_op = 1;
  semop(semid, &atomic_op, 1);
}

/*int main(){
  int s = create_sem();
  view_sem(s);

  decrement_sem(s);
  view_sem(s);

  increment_sem(s);
  view_sem(s);
  
  remove_sem(s);
  s = 0;
  }
*/
