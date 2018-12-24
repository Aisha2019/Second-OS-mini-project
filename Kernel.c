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

void IntHandler(int signum);
key_t DupQueue,DdownQueue,PupQueue,PdownQueue;
int main()
{
  pid_t pid;
  
  // Disc Queues 
  DupQueue = msgget(555, IPC_CREAT|0644); 
  DdownQueue = msgget(666, IPC_CREAT|0644); 
  // Process Queues 
  PupQueue = msgget(777, IPC_CREAT|0644); 
  PdownQueue = msgget(888, IPC_CREAT|0644); 
  signal (SIGINT, IntHandler);
  int rec_val ,send_val,i=0;
  struct msgbuff message;
  const double TIMESTAMP = 0.00001;
  double currentTime = 0;
  long ids [4] = {0,0,0,0};
  /*while(rec_val == -1)
  {
    // Recieve ID of Disc
    rec_val = msgrcv(DupQueue, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);
    if(rec_val != -1) { ids[4] = message.mtype;}
  }
  rec_val = -1;*/
 // while(rec_val == -1 && i < 3)
  //{
    
    /*if(rec_val != -1 ) 
    {
      i++; 
      ids[i] = message.mtype; 
      if(i<2)
      rec_val = -1;
      printf("Recieved Process ID");
    }*/
  //}
  while(1){
    // Recieve ID of Process
      rec_val = msgrcv(PupQueue, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);
      if(rec_val == -1) 
        perror("Error in receive ");
      else 
        ids[0] = message.mtype;
      // Recieve ID of Disc
    rec_val = msgrcv(DupQueue, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);
    if(rec_val != -1) { ids[1] = message.mtype;}

    // Send SIGUSR2 every second to all processes
     currentTime += TIMESTAMP;
     if(currentTime>=1)
      {
        //send signal
        for(int j = 0;j<2;j++) { kill(ids[j],SIGUSR2);}
        currentTime = 0;
      }

     // receive Command From Process 
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

void IntHandler(int signum)
{
  
  msgctl(DupQueue, IPC_RMID, (struct msqid_ds *) 0);
  msgctl(DdownQueue, IPC_RMID, (struct msqid_ds *) 0);
  msgctl(PupQueue, IPC_RMID, (struct msqid_ds *) 0);
  msgctl(PdownQueue, IPC_RMID, (struct msqid_ds *) 0);
  printf("Server Shutdown\n");
  exit(0);
}