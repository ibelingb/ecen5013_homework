/* ECEN5013 - HW4
 * Date: 2/28/2019
 * Author: Brian Ibeling
 * About: C program to demonstrate IPC using shared memory between processes.
 * Resources: Utilized the following resourecs when developing the code contained in this file.
 *    - Linux Man Pages
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> /* For file handling */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "payload.h"

/* Define static and global variables */

/* Define Function Prototypes */
void updatePayload(struct Payload* payload, unsigned char cmd, char* msg, unsigned char length);

/* ------------------------------------------------------------- */
int main() {
  char* logName = "shm1_ipc.log";
  char* shmName = "/tmp/shm_ipc";
  FILE* logFile = NULL;
  struct Payload sendPayload = {0};
  struct Payload rcvPayload = {0};
  void* shmPtr = NULL;
  int shmSize = 10*sizeof(struct Payload);
  int shmFd;
  int status = 0;

  // Create/Open shared memory segment
  shmFd = shm_open(shmName, O_CREAT | O_RDWR, 0666);
  // TODO

  // Configure size of shared memory
  ftruncate(shmFd, shmSize);
  // TODO

  // Memory map the shared memory
  shmPtr = mmap(0, shmSize, PROT_WRITE, MAP_SHARED, shmFd, 0);
  if(*shmPtr == -1){
    printf("ERROR: Failed to complete memory mapping for shared memory for SHM Process 1 - exiting.\n");
    return -1;
  }

  // Open Log file
  if ((logFile = fopen(logName, "w")) == NULL){
    printf("ERROR: Failed to open logfile for SHM Process 1 - exiting.\n");
    return -1;
  } 

  // Log current process info to log file
  // TODO
  printf("SHM Process 1 Info:\nPID: {%d} | 1 FD open for SHM.\n", getpid());
  fprintf(logFile, "[%s] SHM Process 1 Info:\nPID: {%d} | 1 FD open for SHM.\n", getTimestamp(), getpid());

  // --------------------------------------------------------------------------------
  // Send payloads to SHM

  updatePayload(&sendPayload, 1, "", 0);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  updatePayload(&sendPayload, 0, "test", 4);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  updatePayload(&sendPayload, 1, "", 0);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);

  // --------------------------------------------------------------------------------
  // Read payload from SHM

  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  // --------------------------------------------------------------------------------
  // Send payloads to SHM

  updatePayload(&sendPayload, 1, "Testing Write 9", 15);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);

  // --------------------------------------------------------------------------------
  // Read payload from SHM

  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  // --------------------------------------------------------------------------------

  // Cleanup
  fflush(logFile);
  fclose(logFile);
  shm_unlink();
}

