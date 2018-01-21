#include "sharedmem.h"

int create_shm(int key){
    //create shared memory
    int shmid = shmget(key, (sizeof(char) * 50), IPC_CREAT | IPC_EXCL | 0600);
    if(shmid == -1)
      printf("could not create shared memory: %s\n",strerror(errno));
    else
      printf("shared memory created: %d\n",shmid);
    return shmid;
}

void remove_shm(int shmid){
    //remove shared memory
    //shmid = shmget(key, sizeof(int), 0600);
    int s = shmctl(shmid,IPC_RMID,0);
    if(s == -1)
      printf("could not remove shared memory: %s\n",strerror(errno));
    else{
      printf("memory removed: %d\n", s);
    }
}

//hehe mem leak
char * get_shm(int shmid){
  //printf("shmid: %d\n",shmid);
  char *shmem = (char *)shmat(shmid,0,0);

  if(shmem == (char *)-1){
    printf("Could not attach shared memory to pointer: %s\n",strerror(errno));
    return NULL;
  }
  else{
    printf("memory attached\n");
  }

  char *word = (char *)calloc(50,sizeof(char));

  strcpy(word,shmem);
  printf("word: %s\n",word);

  //free(word);

  int success = shmdt(shmem);
  if(success == -1)
    printf("failed for some reason: %s\n",strerror(errno));
  else
    printf("success detachement\n");

  return word;
 
}

void set_shm(char *word,int shmid){
  printf("sharing word: %s\n",word);
  //printf("shmid: %d\n",shmid);
  
  char *shmem = (char *)shmat(shmid,0,0);

  if(shmem == (char *)-1){
    printf("Could not attach shared memory to pointer: %s\n",strerror(errno));
    return;
  }
  else{
    printf("memory attached\n");
  }


  strcpy(shmem,word);
  

  int success = shmdt(shmem);
  if(success == -1)
    printf("failed for some reason: %s\n",strerror(errno));
  else
    printf("success detachment\n");

  
}

int main(){
  int id = create_shm(835);
  set_shm("hi there",id);
  printf("word: %s\n",get_shm(id));
  remove_shm(id);
}
