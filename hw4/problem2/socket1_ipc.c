/* ECEN5013 - HW4
 * Date: 2/28/2019
 * Author: Brian Ibeling
 * About: C program to demonstrate IPC using pipes between processes.
 * Resources: Utilized the following resourecs when developing the code contained in this file.
 *    - 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> /* For file handling */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "payload.h"

/* Define static and global variables */

/* Define Function Prototypes */
void updatePayload(struct Payload* payload, unsigned char cmd, char* msg, unsigned char length);

/* ------------------------------------------------------------- */
int main() {
  char* logName = "socket1_ipc.log";
  char* socketPath = "/tmp/ipc_socket";
  FILE* logFile = NULL;
  struct Payload sendPayload = {0};
  struct Payload rcvPayload = {0};
  int sockfd;
  int sockfd2;
  int status;
  unsigned int cliLen;
  struct sockaddr_un servAddr;
  struct sockaddr_un cliAddr;

  // Create socket
  sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if(sockfd == -1){
    printf("ERROR: Failed to create socket for Process 1 - exiting.\n");
    return -1;
  }

  // Set properties and bind socket
  servAddr.sun_family = AF_UNIX;
  strcpy(servAddr.sun_path, socketPath);
  status = bind(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr));
  if(status == -1){
    printf("ERROR: Failed to bind socket for Process 1 - exiting.\n");
    return -1;
  }

  // Listen for client connection
  status = listen(sockfd, 5);
  if(status == -1){
    printf("ERROR: Failed to listen successfully for socket on Process 1 - exiting.\n");
    return -1;
  }

  // Accept client connection
  cliLen = sizeof(cliAddr);
  sockfd2 = accept(sockfd, (struct sockaddr*)&cliAddr, &cliLen);
  if(sockfd2 == -1){
    printf("ERROR: Failed to accept client connection for socket on Process 1 - exiting.\n");
    return -1;
  }

  // Open Log file
  if ((logFile = fopen(logName, "w")) == NULL){
    printf("ERROR: Failed to open logfile for Socket Process 1 - exiting.\n");
    return -1;
  } 

  // Log current process info to log file
  printf("Socket Process 1 Info:\nPID: {%d} | Log file and 2 Socket resources allocated.\n", getpid());
  fprintf(logFile, "[%s] Socket Process 1 Info:\nPID: {%d} | Log file and 2 Socket resources allocated.\n", getTimestamp(), getpid());

  // --------------------------------------------------------------------------------
  // Send payloads to client

  updatePayload(&sendPayload, 1, "", 0);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  send(sockfd2, &sendPayload, sizeof(sendPayload), 0);
  updatePayload(&sendPayload, 0, "test", 4);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  send(sockfd2, &sendPayload, sizeof(sendPayload), 0);
  updatePayload(&sendPayload, 1, "", 0);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  send(sockfd2, &sendPayload, sizeof(sendPayload), 0);

  // --------------------------------------------------------------------------------
  // Read payload from FIFO
  
  recv(sockfd2, &rcvPayload, sizeof(rcvPayload), 0);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  recv(sockfd2, &rcvPayload, sizeof(rcvPayload), 0);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  recv(sockfd2, &rcvPayload, sizeof(rcvPayload), 0);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  recv(sockfd2, &rcvPayload, sizeof(rcvPayload), 0);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  recv(sockfd2, &rcvPayload, sizeof(rcvPayload), 0);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  // --------------------------------------------------------------------------------
  // Send payloads to FIFO

  updatePayload(&sendPayload, 1, "Testing Write 9", 15);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  send(sockfd2, &sendPayload, sizeof(sendPayload), 0);

  // --------------------------------------------------------------------------------
  // Read payload from FIFO

  recv(sockfd2, &rcvPayload, sizeof(rcvPayload), 0);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  // --------------------------------------------------------------------------------

  // Cleanup
  fflush(logFile);
  fclose(logFile);
  close(sockfd2);
  close(sockfd);
  unlink(socketPath); 
}

