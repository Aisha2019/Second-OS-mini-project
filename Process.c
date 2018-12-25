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
// UP & Down Queues
key_t upQueue,downQueue;
// Clock 
int clk = 0;

void Handler1(int signum);
int main()
{
  int pid = getpid()%10000;

  // Message Queues
  upQueue = msgget(1341, IPC_CREAT|0644); 
  downQueue = msgget(1343, IPC_CREAT|0644); 

  // Signal Handler
  signal (SIGUSR2, Handler1);
  
  char str[64],str2[64];
  int rec_val,send_val;
  struct msgbuff message;
  message.mtype = pid;
  // Read Commands From File
  printf("Enter File Name : \n");
  char* filename[100];
  scanf("%s",filename);
  FILE *file;
  file = fopen(filename, "r"); // read mode
  // test for file not existing. 
  if (file == NULL) 
  {   
    printf("Error! Could not open file\n"); 
    exit(-1); 
  }
   
  int i = 0,cycle;
    // Send PID to Kernel
  send_val = msgsnd(upQueue, &message, sizeof(message.mtext), !IPC_NOWAIT);
  if(send_val == -1)
    perror("Errror in send");
  else printf("%d\n",pid);
  while(1){
    while ( fscanf(file, "%s", & str ) == 1 )  
    { 
      if(i == 0) // Clock Cycle
      {
        cycle = atoi(str);
        i++;
      }
      else if(i == 1) // Command
      {
        str2[0] = str[0];
        i++;
      }
      else if(i == 2) // Data
      {
        i = 0;
        str2[1] = ' ';
        strcat(str2,str);
        if(cycle == clk)
        strcpy(message.mtext,str2);
          send_val = msgsnd(upQueue, &message, sizeof(message.mtext), !IPC_NOWAIT);
          if(send_val == -1)
            perror("Errror in send");
      }
    
    // Get Response From Kernel
    rec_val = msgrcv(downQueue, &message, sizeof(message.mtext), pid , !IPC_NOWAIT);  
    if(rec_val == -1)
      perror("Error in receive");
    else if(message.mtext[0]=='0')
      printf("%s\n","Successful ADD");
    else if(message.mtext[0]=='1')
      printf("%s\n","Successful DELETE");
    else if(message.mtext[0]=='2')
      printf("%s\n","UNSuccessful ADD");
    else if(message.mtext[0]=='3')
      printf("%s\n","UNSuccessful DELETE");
    }
  }
  return 0;
}
// Handler of SIGUSR2 : Increment CLK
void Handler1(int signum){

  //increment Clock
  clk++;
}

