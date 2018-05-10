#include <errno.h>
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

struct mymsg{
  long mtype;
  char mtext[1];
} mymsg_t;

int main () {
  int childId = getpid();
  int stuffToDo = rand(childId) % 50000 + 1;
// wait for msg

  
}
  return 0;
}
