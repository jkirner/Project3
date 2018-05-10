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
	info myinfo;
	int id;
	int msgqid;
	int msgqid2;
	int msgqid3;
	int msgqid4;
	mymsg dummyMes;
	myinfo.childPid = int(argv[1]);
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
	myinfo.childPid = getpid();

	msgqid = msgget (412, IPC_CREAT | 0777);
	msgqid2 = msgget (413, IPC_CREAT | 0777);
	msgqid3 = msgget (414, IPC_CREAT | 0777);
	msgqid4 = msgget (415, IPC_CREAT | 0777);
	
	if ((msgrcv (msgqid, &dummyMes, sizeof(dummyMes), 0, 0)) == -1) {
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
	myinfo.bornSec = sharedClock->sec;
	myinfo.bornNano = sharedClock->nano;
	msgsnd (msgqid4, &dummyMes, sizeof(dummyMes),	0);
	srand(myinfo.childPid);
	int stuffToDo = rand()%1000000 + 1;
	myinfo.worked = 0;
	while(1){
		if ((msgrcv (msgqid2, &dummyMes, sizeof(dummyMes), 0, 0)) == -1) {
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
		if(sharedClock->sec > int(argv[2])){
			break;
		}
		if(myinfo.worked - stuffToDo > 100000){
			sharedClock->nano += 100000;
			if(sharedClock->nano > 999999999){
				sharedClock->nano -= 1000000000;
				sharedClock->sec ++;
			}
			myinfo.worked += 100000;
			if(sharedClock->sec > int(argv[2])){
				break;
			}
			msgsnd (msgqid2, &dummyMes, sizeof(dummyMes),	0);
		}
		else{
			sharedClock->nano += (myinfo.worked - stuffToDo);
			if(sharedClock->nano > 999999999){
				sharedClock->nano -= 1000000000;
				sharedClock->sec ++;
			}
			myinfo.worked += (myinfo.worked - stuffToDo);
			break;
		}
	}
	dummyMes.myinfo = myinfo;
	msgsnd (msgqid3, &dummyMes, sizeof(dummyMes),	0);
// wait for msg
	return 0;
}
