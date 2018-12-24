#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
char* list[10] = {"","","","","","","","","",""}; // Disc has 10 slots
// UP & Down Queues
key_t upQueue,downQueue;
// Clock 
int clk = 0;
struct msgbuff
{
   long mtype;
   char mtext[64];
};
void Handler1(int signum);
void Handler2(int signum);
int Add(char* msg);
int Delete(int id);
int getFirstFree();
int countFreeSlots();
void Excute_msg(char* message);


int main()
{
  int pid = getpid()%10000;

	// Initialize UP & Down Queues
  upQueue = msgget(777, IPC_CREAT|0644); 
  downQueue = msgget(888, IPC_CREAT|0644); 

	//Signals Handeler
  signal (SIGUSR1, Handler1);
  signal (SIGUSR2, Handler2);

  char str[64];
  int rec_val,send_val;
  struct msgbuff message;
  message.mtype = pid;
  while(1){

    rec_val = msgrcv(downQueue, &message, sizeof(message.mtext), pid , !IPC_NOWAIT);  
    if(rec_val == -1)
      perror("Error in receive");
    else
      Excute_msg(message.mtext);
  }
  return 0;
}
// Handler of SIGUSR1 : Send # of Free Slots (Disc Status)
void Handler1(int signum){
    int num = countFreeSlots();
  //send number of free slots
    struct msgbuff message;
    sprintf(message.mtext, "%s%d", message.mtext, num);
    int send_val = msgsnd(upQueue, &message, sizeof(message.mtext), !IPC_NOWAIT);
    if(send_val == -1)
    	perror("Error in send Disc Status");
}
// Handler of SIGUSR2 : Increment CLK
void Handler2(int signum){

  //increment Clock
  clk++;
}
// Add To Disc
int Add(char* msg){
	// Search for free slot then insert msg
  int id = getFirstFree();
  if(id == -1) return -1;
  strcpy(list[id],msg);
  // delay 3 seconds
  int current_Clk = clk;
  while(clk != current_Clk+3){}
  return id;
}
// Delete From Disc
int Delete(int id){
  // Free slot with given id
  if(id >= 10 || list[id] == "") return -1;
  list[id] = "";
  // delay 1 second
  int current_Clk = clk;
  while(clk != current_Clk+1){}
  return id;
}
// Get the first Free Slot in Disc
int getFirstFree(){
  for(int i=0;i<10;i++) 
    if(list[i]=="")
      return i;
  return -1;
}
// Return Number of Free Slots in Disc
int countFreeSlots(){
  int count = 0;
  for(int i=0;i<10;i++) 
    {
      if(list[i]=="")
      count++;
    }
  return count;
}
// Check Type of Command Then Excute it
void Excute_msg(char* message){
  if(message[0] == 'D') // Delete Command
  {
    int num = Delete(message[2]-48);
    // Send Delete Result
    struct msgbuff message;
    sprintf(message.mtext, "%s%d", message.mtext, num);
    int send_val = msgsnd(upQueue, &message, sizeof(message.mtext), !IPC_NOWAIT);
    if(send_val == -1)
    	perror("Error in send Deletion Result");
  }
  else if(message[0] == 'A') // Add Command
  {
    int num = Add(message[2]);
    // Send Add Result
    struct msgbuff message;
    sprintf(message.mtext, "%s%d", message.mtext, num);
    int send_val = msgsnd(upQueue, &message, sizeof(message.mtext), !IPC_NOWAIT);
    if(send_val == -1)
    	perror("Error in send Addition Result");
  }
  else
  {
    perror("Inappropriate Message");
  }
}
