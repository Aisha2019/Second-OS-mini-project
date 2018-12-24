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

  int rec_val,send_val;
  struct msgbuff message;
  const double TIMESTAMP = 0.00001;
  double currentTime = 0;
  while(1){
     currentTime += TIMESTAMP;
     if(currentTime>=1)
      {
        //send signal
        currentTime = 0;
      }

     // receive Command From Process 
     rec_val = msgrcv(PupQueue, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);  
     if(rec_val == -1)
      perror("Error in receive");
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

