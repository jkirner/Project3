#include <errno.h>
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

struct info{
  int childPid;
  int worked;
  int born;
  int died;
}

struct mymsg{
  info myInfo;
  bool wasKilled = 0;
}

int main (int argc, char *argv[]) {
  pid_t childpid = 0;
  int spawns = 5;
  int secs = 20;
  char fileName[100]= "log.txt";
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
  int *sharedSeconds = 0;
  int *sharedNano = 0;
  int id;

  if((id = shmget(IPC_PRIVATE, sizeof(int), PERM)) == -1) {
    perror((%s: Error: Failed to attached shared memory segment", argv[0]));
    return 1;
  }

  if((sharedSecond = (int *)shmat(id, NULL, 0)) == (void *)-1){
    perror((%s: Error: Failed to attach shared memory segment", argv[0]));
    if(shmctl(id, IPC_RMID, NULL) == -1)
      perror((%s: Error: Failed to remove memory segment", argv[0]));
    return 1;
  }
  if((sharedNano = (int *)shmat(id, NULL, 0)) == (void *)-1{ 
    perror((%s: Error: Failed to attach shared memory segment", argv[0]));
    if(shmctl(id, IPC_RMID, NULL) == -1)
      perror((%s: Error: Failed to remove memory segment", argv[0]));
    return 1;
  }
  int a;
  int spawnCount = 0;
  int totalSpawns = 0;
  int msgqid;
// create message queue
  msgqid = msgget(IPC_PRIVATE, MSGPERM|IPC_CREAT|IPCEXCL);
  if (msgqid < 0) {
    perror(("%s: Error: Failed to create message queue",argv[0]));
    return 1;
  }
  printf("msg queue is %d\n", msgqid);
  
  while (spawnCount < spawns){
    if((childpid = fork()) == -1){
      perror(("%s: Error: Failed to create child process", argv[0]));
      if (detachandremove(id, sharedSeconds) == -1)
        perror(("%s: Error: Failed to destory shared memory segment"));
      if (detachandremove(id, sharedNano) == -1)
        perror(("%s: Error: Failed to destory shared memory segment"));
      return 1;
    }
    if (childpid == 0){
      break;
    }
    
    spawnCount++;
  }
//  run a child
  if (childpid  == 0){
    char *args[]={"./child",NULL};
    execvp(args[0], args); 
    break;
  }
  if(spawnCount != spawns){
    perror(("%s: Error: Spawn count did not work as planned", argv[0]));
    if (detachandremove(id, sharedSeconds) == -1)
      perror(("%s: Error: Failed to destory shared memory segment"));
    if (detachandremove(id, sharedNano) == -1)
      perror(("%s: Error: Failed to destory shared memory segment"));
    return 1;
  }
  if (childpid == 0) {
    perror(("%s: Error: Unexpected child", argv[0]));
    if (detachandremove(id, sharedSeconds) == -1)
      perror(("%s: Error: Failed to destory shared memory segment"));
    if (detachandremove(id, sharedNano) == -1)
      perror(("%s: Error: Failed to destory shared memory segment"));
    return 1;
  }
  while (spawnCount > 0){
  wait(NULL);
}
  return 0;
}
