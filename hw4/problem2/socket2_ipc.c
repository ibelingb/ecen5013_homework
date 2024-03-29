/* ECEN5013 - HW4
 * Date: 2/28/2019
 * Author: Brian Ibeling
 * About: C program to demonstrate IPC using sockets between processes.
 * Resources: Utilized the following resourecs when developing the code contained in this file.
 *    - 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> /* For file handling */
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "payload.h"

/* Define static and global variables */
char* logName = "socket2_ipc.log";
FILE* logFile = NULL;
int sockfd;

/* Define Function Prototypes */
void updatePayload(struct Payload* payload, unsigned char cmd, char* msg, unsigned char length);
void sigHandler(int sig);

/* ------------------------------------------------------------- */
int main() {
  char* socketPath = "/tmp/ipc_socket";
  struct Payload sendPayload = {0};
  struct Payload rcvPayload = {0};
  struct sockaddr_un servAddr;
  struct sigaction sigAction;
  int status;

  // Register Signal Handler
  sigAction.sa_handler = sigHandler;
  sigAction.sa_flags = 0;
  sigaction(SIGINT, &sigAction, NULL);

  // Create client socket
  sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if(sockfd == -1){
    printf("ERROR: Failed to create client socket for Process 2 - exiting.\n");
    return -1;
  }

  servAddr.sun_family = AF_UNIX;
  strcpy(servAddr.sun_path, socketPath);
  status = connect(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr));
  if(status == -1){
    printf("ERROR: Failed to connect to socket from Process 2 - exiting.\n");
    return -1;
  }

  // Open Log file
  if ((logFile = fopen(logName, "w")) == NULL){
    printf("ERROR: Failed to open logfile for Socket Process 2 - exiting.\n");
    return -1;
  }
  
  // Log current process info to log file
  printf("Socket Process 2 Info:\nPID: {%d} | Log file and Socket resources allocated.\n", getpid()); 
  fprintf(logFile, "[%s] Socket Process 2 Info:\nPID: {%d} | Log file and socket resources allocated.\n", getTimestamp(), getpid());

  // --------------------------------------------------------------------------------
  // Read payload from server Socket
  
  recv(sockfd, &rcvPayload, sizeof(rcvPayload), 0);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  recv(sockfd, &rcvPayload, sizeof(rcvPayload), 0);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  recv(sockfd, &rcvPayload, sizeof(rcvPayload), 0);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  // --------------------------------------------------------------------------------
  // Send payloads to Socket

  updatePayload(&sendPayload, 3, "", 0); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  send(sockfd, &sendPayload, sizeof(sendPayload), 0);

  updatePayload(&sendPayload, 0, "update", 6); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  send(sockfd, &sendPayload, sizeof(sendPayload), 0);

  updatePayload(&sendPayload, 0, "", 0); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  send(sockfd, &sendPayload, sizeof(sendPayload), 0);

  // Delay to demonstrate Socket read to block
  sleep(2);

  updatePayload(&sendPayload, 0, "TESTING", 7); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  send(sockfd, &sendPayload, sizeof(sendPayload), 0);

  updatePayload(&sendPayload, 1, "", 0); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  send(sockfd, &sendPayload, sizeof(sendPayload), 0);

  // --------------------------------------------------------------------------------
  // Read payload from Socket

  recv(sockfd, &rcvPayload, sizeof(rcvPayload), 0);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  // --------------------------------------------------------------------------------
  // Send payloads to Socket

  updatePayload(&sendPayload, 99, "Ten!!", 5); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  send(sockfd, &sendPayload, sizeof(sendPayload), 0);

  // --------------------------------------------------------------------------------
  // Alternate reading and writing

  recv(sockfd, &rcvPayload, sizeof(rcvPayload), 0);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  updatePayload(&sendPayload, 1, "P2 Test1", 8); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  send(sockfd, &sendPayload, sizeof(sendPayload), 0);

  recv(sockfd, &rcvPayload, sizeof(rcvPayload), 0);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  updatePayload(&sendPayload, 1, "P2 Test2", 8); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  send(sockfd, &sendPayload, sizeof(sendPayload), 0);

  recv(sockfd, &rcvPayload, sizeof(rcvPayload), 0);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  updatePayload(&sendPayload, 1, "P2 Test3", 8); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  send(sockfd, &sendPayload, sizeof(sendPayload), 0);

  recv(sockfd, &rcvPayload, sizeof(rcvPayload), 0);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  updatePayload(&sendPayload, 1, "P2 Test4", 8); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  send(sockfd, &sendPayload, sizeof(sendPayload), 0);

  recv(sockfd, &rcvPayload, sizeof(rcvPayload), 0);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  updatePayload(&sendPayload, 1, "P2 Test5", 8); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  send(sockfd, &sendPayload, sizeof(sendPayload), 0);

  recv(sockfd, &rcvPayload, sizeof(rcvPayload), 0);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  // --------------------------------------------------------------------------------
  // Cleanup
  printf("[%s] Socket Process 2 Complete.\n", getTimestamp());
  fprintf(logFile, "[%s] Socket Process 2 Complete.\n", getTimestamp());

  fflush(logFile);
  fclose(logFile);
  close(sockfd);
}

// --------------------------------------------------------------------------------
void sigHandler(int sig){
  // Ensure logFile is closed before opening
  fflush(logFile);
  fclose(logFile);

  // Open log to capture that signal event has occurred
  if ((logFile = fopen(logName, "w")) == NULL){
    printf("ERROR: Failed to open logfile in Socket Process 2 sigHandler() - Exiting and cleaning resources.\n");
  }

  printf("[%s] SIGINT signal received! Killing Socket 2 process {%d}\n.", getTimestamp(), getpid());
  fprintf(logFile, "[%s] SIGINT signal received! Killing Socket 2 process {%d}\n.", getTimestamp(), getpid());

  // cleanup resources
  fflush(logFile);
  fclose(logFile);
  close(sockfd);

  exit(1);
}
