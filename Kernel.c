#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>

struct msgbuff
{
   long mtype;
   char mtext[256];
};


key_t upQueue,downQueue;
int main()
{
  pid_t pid;
  

  signal (SIGINT, intHandler);

  upQueue = msgget(777, IPC_CREAT|0644); 
  downQueue = msgget(888, IPC_CREAT|0644); 

  int rec_val,send_val;
  struct msgbuff message;
  while(1){
     /* receive all types of messages */
     rec_val = msgrcv(upQueue, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);  
     if(rec_val == -1)
      perror("Error in receive");
    else
      {
        conv(message.mtext,strlen(message.mtext));
        send_val = msgsnd(downQueue, &message, sizeof(message.mtext), !IPC_NOWAIT);
        if(send_val == -1)
          perror("Errror in send");
      }
  }
  return 0;
}


void intHandler(int signum){

  msgctl(upQueue, IPC_RMID, (struct msqid_ds *) 0);
  msgctl(downQueue, IPC_RMID, (struct msqid_ds *) 0);

  printf("Server Shutdown\n");
  exit(0);
}
