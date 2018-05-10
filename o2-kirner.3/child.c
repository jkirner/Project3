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
typedef struct info{
  int childPid;
  int worked;
  int bornSec;
  int bornNano;
  int diedSec;
  int diedNano
};
typedef struct clock{
  int nano = 0;
  int sec = 0;
};
typedef struct mymsg{
  long type;
  info myinfo;
  bool wasKilled = 0;
};

int main () {
  info myinfo;
  int id;
  clock *sharedClock;
  
  
  if((id = shmget(411, sizeof(clock), IPC_CREAT|0666)) == -1) {
    perror((%s: Error: Failed to attached shared memory segment", argv[0]));
    return 1;
  }
  if((sharedClock= (clock *)shmat(id, NULL, 0)) == (void *)-1){
    perror((%s: Error: Failed to attach shared memory segment", argv[0]));
    if(shmctl(id, IPC_RMID, NULL) == -1)
      perror((%s: Error: Failed to remove memory segment", argv[0]));
    return 1;
  }
  myinfo.childPid = getpid();
  msgqid = msgget (412, IPC_CREAT | 0777);
  msgqid2 = msgget (413, IPC_CREAT | 0777);
  
  if ((msgrcv (msgqid, &dummyMes, sizeof(dummyMes), 0)) == -1) {
      perror(("%s: Error: Failed to recieve message"));
	  if (detachandremove(id, sharedClock) == -1)
        perror(("%s: Error: Failed to destory shared memory segment"));
	  return 1;
    }
  printf("%d %d %d", myinfo.childPid, sharedClock.sec, sharedClock.nano);
  if ((msgrcv (msgqid2, &dummyMes, sizeof(dummyMes), 0)) == -1) {
      perror(("%s: Error: Failed to recieve message"));
	  if (detachandremove(id, sharedClock) == -1)
        perror(("%s: Error: Failed to destory shared memory segment"));
	  return 1;
    }
  int stuffToDo = rand(childId) % 100000 + 1;
  sharedClock.nano += stuffToDo;
  mymsg dummyMes;
  dummyMes.myinfo = myinfo;
  msgsnd (msgqid, &dummyMes, sizeof(dummyMes),  0);
// wait for msg

  
}
  return 0;
}
