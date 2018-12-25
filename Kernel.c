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
   char mtext[64];
};

void intHandler(int signum);

key_t DupQueue,DdownQueue,PupQueue,PdownQueue;

int main()
{
  pid_t pid;
  
  // Disc Queues 
  DupQueue = msgget(32321, IPC_CREAT|0644); 
  DdownQueue = msgget(1231, IPC_CREAT|0644); 
  // Process Queues 
  PupQueue = msgget(1341, IPC_CREAT|0644); 
  PdownQueue = msgget(1343, IPC_CREAT|0644); 

  signal (SIGINT, intHandler);

  int rec_val ,send_val,i=0;
  struct msgbuff message;
  const double TIMESTAMP = 0.00000001;
  double currentTime = 0;
  long diskID = 0;
  long ids [3] = {0,0,0};
  // Recieve ID of Disk
  rec_val = msgrcv(DupQueue, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);
  diskID = message.mtype;
  // Recieve ID of Processes
  do{
    rec_val = msgrcv(PupQueue, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);
    ids[i] = message.mtype; 
    i++; 
  }while(i<3);
  while(1){
    // Send SIGUSR2 every second to all processes
     currentTime += TIMESTAMP;
     if(currentTime>=1)
      {
        //send signal to processes
        for(int j = 0;j<3;j++) { kill(ids[j],SIGUSR2);}
        //send signal to disk
        kill(diskID,SIGUSR2);
        currentTime = 0;
      }

    /**********This part needs some checking**************/
     //receive Command From Process 
      rec_val = msgrcv(PupQueue, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);  
      if(rec_val == -1)
       perror("Error in receive ");
     else
       { 
         // Send Signal to recieve disc status
         //Recieve Disc Status
         // If successfully recieved , send command to disc
         send_val = msgsnd(DdownQueue, &message, sizeof(message.mtext), !IPC_NOWAIT);
         if(send_val == -1)
           perror("Errror in send");
         else
         {
           // If successfully sent , recieve response from disc
           rec_val = msgrcv(DupQueue, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);  
           if(rec_val == -1)
             perror("Error in receive");
           else 
           {
             // If successfully recieved , send response to Process
             send_val = msgsnd(DdownQueue, &message, sizeof(message.mtext), !IPC_NOWAIT);
             if(send_val == -1)
               perror("Errror in send");
           }
         }
       }
  }
  return 0;
}

void intHandler(int signum){

  msgctl(DupQueue, IPC_RMID, (struct msqid_ds *) 0);
  msgctl(DdownQueue, IPC_RMID, (struct msqid_ds *) 0);
  msgctl(PupQueue, IPC_RMID, (struct msqid_ds *) 0);
  msgctl(PdownQueue, IPC_RMID, (struct msqid_ds *) 0);

  printf("Disk Shutdown\n");
  exit(0);
}
