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
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/mman.h>

#include "payload.h"

/* Define static and global variables */
char* logName = "shm2_ipc.log";
char* shmName = "/shm_ipc";
FILE* logFile = NULL;
int shmFd;

/* Define Function Prototypes */
void updatePayload(struct Payload* payload, unsigned char cmd, char* msg, unsigned char length);
void sigHandler(int sig);

/* ------------------------------------------------------------- */
int main() {
  struct Payload sendPayload = {0};
  struct Payload rcvPayload = {0};
  struct sigaction sigAction;
  void* shmPtr = NULL;
  int payloadSize = sizeof(struct Payload);
  int shmSize = 10*payloadSize;

  // Register Signal Handler
  sigAction.sa_handler = sigHandler;
  sigAction.sa_flags = 0;
  sigaction(SIGINT, &sigAction, NULL);

  // Open shared memory 
  shmFd = shm_open(shmName, O_RDWR, 0666);
  if(shmFd == -1){
    printf("SHM Process 2 failed to open shared memory - exiting.\n");
    return -1;
  }

  // Memory map the shared memory
  shmPtr = mmap(0, shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
  if(*(int *)shmPtr == -1){
    printf("ERROR: Failed to complete memory mapping for shared memory for SHM Process 1 - exiting.\n");
    return -1;
  }

  // Open Log file
  if ((logFile = fopen(logName, "w")) == NULL){
    printf("ERROR: Failed to open logfile for SHM Process 2 - exiting.\n");
    return -1;
  }
  
  // Log current process info to log file
  printf("SHM Process 2 Info:\nPID: {%d} | 1 FD open for SHM, Log file open.\n", getpid());
  fprintf(logFile, "[%s] SHM Process 2 Info:\nPID: {%d} | 1 FD open for SHM, Log file open.\n", getTimestamp(), getpid());

  // --------------------------------------------------------------------------------
  // Read 5 payloads from SHM

  // Read SHM[0]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*0)), payloadSize);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[1]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*1)), payloadSize);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[2]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*2)), payloadSize);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[3]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*3)), payloadSize);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[4]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*4)), payloadSize);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  // --------------------------------------------------------------------------------
  // Send next 5 payloads to SHM

  // Write SHM[5]
  updatePayload(&sendPayload, 1, "", 0);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*5)), &sendPayload, payloadSize);
  // Write SHM[6]
  updatePayload(&sendPayload, 0, "test", 4);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*6)), &sendPayload, payloadSize);
  // Write SHM[7]
  updatePayload(&sendPayload, 1, "", 0);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*7)), &sendPayload, payloadSize);
  // Write SHM[8]
  updatePayload(&sendPayload, 0, "Message Number 4", 16);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*8)), &sendPayload, payloadSize);
  // Write SHM[9]
  updatePayload(&sendPayload, 0, "", 0);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*9)), &sendPayload, payloadSize);

  // --------------------------------------------------------------------------------
  // Delay to allow SHM Process 1 to write
  printf("10 sec delay to allow SHM Process 1 write data and next 10 payloads.\n");
  sleep(10);

  // --------------------------------------------------------------------------------

  // Read SHM[0]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*0)), payloadSize);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[1]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*1)), payloadSize);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[2]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*2)), payloadSize);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[3]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*3)), payloadSize);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[4]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*4)), payloadSize);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[5]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*5)), payloadSize);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[6]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*6)), payloadSize);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[7]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*7)), payloadSize);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[8]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*8)), payloadSize);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[9]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*9)), payloadSize);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  // --------------------------------------------------------------------------------
  // Send next 5 payloads to SHM

  // Write SHM[0]
  updatePayload(&sendPayload, 1, "P2 Test1", 8);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*0)), &sendPayload, payloadSize);
  // Write SHM[1]
  updatePayload(&sendPayload, 0, "P2 Test2", 8);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*1)), &sendPayload, payloadSize);
  // Write SHM[2]
  updatePayload(&sendPayload, 1, "P2 Test3", 8);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*2)), &sendPayload, payloadSize);
  // Write SHM[3]
  updatePayload(&sendPayload, 0, "P2 Test4", 8);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*3)), &sendPayload, payloadSize);
  // Write SHM[4]
  updatePayload(&sendPayload, 0, "P2 Test5", 8);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*4)), &sendPayload, payloadSize);

  sleep(5); // Delay to allow P1 to complete before unlinking
  // --------------------------------------------------------------------------------
  // Cleanup
  printf("[%s] SHM Process 2 Complete.\n", getTimestamp());
  fprintf(logFile, "[%s] SHM Process 2 Complete.\n", getTimestamp());

  fflush(logFile);
  fclose(logFile);
  close(shmFd);
  shm_unlink(shmName);
}

// --------------------------------------------------------------------------------
void sigHandler(int sig){
  // Ensure logFile is closed before opening
  fflush(logFile);
  fclose(logFile);

  // Open log to capture that signal event has occurred
  if ((logFile = fopen(logName, "w")) == NULL){
    printf("ERROR: Failed to open logfile in SHM Process 2 sigHandler() - Exiting and cleaning resources.\n");
  }

  printf("[%s] SIGINT signal received! Killing SHM 2 process {%d}\n.", getTimestamp(), getpid());
  fprintf(logFile, "[%s] SIGINT signal received! Killing SHM 2 process {%d}\n.", getTimestamp(), getpid());

  // cleanup resources
  fflush(logFile);
  fclose(logFile);
  close(shmFd);
  shm_unlink(shmName);

  exit(1);
}
