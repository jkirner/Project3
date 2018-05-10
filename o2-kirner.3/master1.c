#include <errno.h>
#include <sys/shm.h>
#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <getopt.h>
#include <ctype.h>
#include <limits.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/errno.h>
typedef struct{
  int childPid;
  int worked;
  int bornSec;
  int bornNano;
  int diedSec;
  int diedNano;
}info;
typedef struct{
  int nano;
  int sec;
}clockS;
typedef struct{
  long type;
  info myinfo;
}mymsg;

int main (int argc, char *argv[]) {
  pid_t childpid;
  int spawns = 5;
  int secs = 20;
  char* fileName= "log.txt";
  int option;

// This allows for the -n and help if you dont know what to do
  while((option = getopt(argc, argv, "h:l:s:t")) != -1){
      switch(option){
        case 's': spawns = atoi(optarg);
        break;
	    case 'l': fileName = optarg;
	    break;
	    case 't': secs = atoi(optarg);
	    break; 

        case 'h': printf("This program has a master program that spawns at default 5 processes.  To change this number you may include -s x where x is the number desired.  The program will also used the file log.txt as default unless you include -l filename less than 100 characters long.  Lastly the program will automatically terminate itself and all children after 20 secounds unless you include -t z where z is the umber of seconds desired.");
        return 1;
        
        default: 
        break;
      }
  }
//error if the max processes is a positive number  
  if (spawns < 1) {
    perror (("%s: Error: Command line arguement following -s must be a numerical value greater than 0", argv[0]));
    return 1;
  }
  if (secs < 1) {
    perror (("%s: Error: Command line arguement following -t mus be a numerical value greater than 0", argv[0]));
    return 1;
  }
  int id;

  if((id = shmget(411, sizeof(clockS), IPC_CREAT|0666)) == -1) {
    perror(("%s: Error: Failed to attached shared memory segment", argv[0]));
    return 1;
  }
  clockS *sharedClock= (clockS *)shmat(id, NULL, 0);
  if(sharedClock == (void *)-1){
    perror(("%s: Error: Failed to attach shared memory segment", argv[0]));
    if(shmctl(id, IPC_RMID, NULL) == -1)
      perror(("%s: Error: Failed to remove memory segment", argv[0]));
    return 1;
  }
  int a;
  int spawnCount = 0;
  int totalSpawns = 0;
  int msgqid;
  int msgqid2;
  int msgqid3;
  mymsg dummyMes;
  dummyMes.type = 2;
// create message queue
  msgqid = msgget (412, IPC_CREAT | 0777);
  msgqid2 = msgget (413, IPC_CREAT | 0777);
  msgqid3 = msgget (414, IPC_CREAT | 0777);
  if (msgqid < 0) {
    perror(("%s: Error: Failed to create message queue",argv[0]));
    return 1;
  }
  printf("msg queue is %d\n", msgqid);
  
  while (spawnCount < spawns){
	childpid = fork();
    if(childpid == -1){
      perror(("%s: Error: Failed to create child process", argv[0]));
      if (shmdt (sharedClock)) {
        perror (("%s: Error: Failed to detach shared memory", argv[0]));
        exit(EXIT_FAILURE);
      }
      if ((shmctl (id, IPC_RMID, NULL)) == -1) {
        perror (("%s: Error: Failed to destroy shared memory", argv[0]));
        exit(EXIT_FAILURE);
      }
      return 1;
    }
    if (childpid == 0){
      break;
    }
	printf("Creating new child pid %d at my time %d.%d", childpid, sharedClock->sec, sharedClock->nano);
    msgsnd (msgqid, &dummyMes, sizeof(dummyMes),  0);
	spawnCount++;
  }
//  run a child
  if (childpid  == 0){
	int name = getpid();
    char *args[]={"./child", name};
    execvp(args[0], args); 
  }
  if(spawnCount != spawns){
    perror(("%s: Error: Spawn count did not work as planned", argv[0]));
    if (shmdt (sharedClock)) {
      perror (("%s: Error: Failed to detach shared memory", argv[0]));
      exit(EXIT_FAILURE);
    }
    if ((shmctl (id, IPC_RMID, NULL)) == -1) {
      perror (("%s: Error: Failed to destroy shared memory", argv[0]));
      exit(EXIT_FAILURE);
    }
    return 1;
  }
  if (childpid == 0) {
    perror(("%s: Error: Unexpected child", argv[0]));
    if (shmdt (sharedClock)) {
      perror (("%s: Error: Failed to detach shared memory", argv[0]));
      exit(EXIT_FAILURE);
    }
    if ((shmctl (id, IPC_RMID, NULL)) == -1) {
      perror (("%s: Error: Failed to destroy shared memory", argv[0]));
      exit(EXIT_FAILURE);
    }
    return 1;
  }
  msgsnd (msgqid2, &dummyMes, sizeof(dummyMes),  0);
  while(spawnCount){
    if ((msgrcv (msgqid3, &dummyMes, sizeof(dummyMes), 0, 0)) == -1) {
      perror(("%s: Error: Failed to recieve message"));
	  if (shmdt (sharedClock)) {
        perror (("%s: Error: Failed to detach shared memory", argv[0]));
        exit(EXIT_FAILURE);
      }
      if ((shmctl (id, IPC_RMID, NULL)) == -1) {
        perror (("%s: Error: Failed to destroy shared memory", argv[0]));
        exit(EXIT_FAILURE);
      }
	  return 1;
    }
	printf("Master: Child pid %d is terminated at my time %d.%d because it reached %d, it was born at %d.%d\n", dummyMes.myinfo.childPid, sharedClock->sec, sharedClock->nano, dummyMes.myinfo.worked, dummyMes.myinfo.bornSec, dummyMes.myinfo.bornNano);
    wait(NULL);
	spawnCount--;
	msgsnd (msgqid2, &dummyMes, sizeof(dummyMes),  0);
  }
  while (spawnCount > 0){
  wait(NULL);
  spawnCount--;
  }
  if (shmdt (sharedClock)) {
    perror (("%s: Error: Failed to detach shared memory", argv[0]));
    exit(EXIT_FAILURE);
  }
  if ((shmctl (id, IPC_RMID, NULL)) == -1) {
    perror (("%s: Error: Failed to destroy shared memory", argv[0]));
    exit(EXIT_FAILURE);
  }
  return 0;
}
