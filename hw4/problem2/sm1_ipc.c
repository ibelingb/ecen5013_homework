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
#include <sys/shm.h>
#include <sys/mman.h>

#include "payload.h"

/* Define static and global variables */

/* Define Function Prototypes */
void updatePayload(struct Payload* payload, unsigned char cmd, char* msg, unsigned char length);

/* ------------------------------------------------------------- */
int main() {
  char* logName = "shm1_ipc.log";
  char* shmName = "/shm_ipc";
  FILE* logFile = NULL;
  struct Payload sendPayload = {0};
  struct Payload rcvPayload = {0};
  void* shmPtr = NULL;
  int payloadSize = sizeof(struct Payload);
  int shmSize = 10*payloadSize; // Create shared memory large enough for 10 messages
  int shmFd;
  int status = 0;

  // Make sure SHM is cleaned up from previous runs
  shm_unlink(shmName);

  // Create/Open shared memory segment
  shmFd = shm_open(shmName, O_CREAT | O_RDWR, 0666);
  // TODO

  // Configure size of shared memory
  ftruncate(shmFd, shmSize);
  // TODO

  // Memory map the shared memory
  shmPtr = mmap(0, shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
  if(*(int *)shmPtr == -1){
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
  printf("SHM Process 1 Info:\nPID: {%d} | 1 FD open for SHM, Log file open.\n", getpid());
  fprintf(logFile, "[%s] SHM Process 1 Info:\nPID: {%d} | 1 FD open for SHM, Log file open.\n", getTimestamp(), getpid());

  // --------------------------------------------------------------------------------
  // Send 5 payloads to SHM

  // Write to SHM[0] for struct Payload object
  updatePayload(&sendPayload, 1, "", 0);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*0)), &sendPayload, payloadSize); 
  // Write SHM[1]
  updatePayload(&sendPayload, 0, "test", 4);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*1)), &sendPayload, payloadSize); 
  // Write SHM[2]
  updatePayload(&sendPayload, 1, "", 0);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*2)), &sendPayload, payloadSize);
  // Write SHM[3]
  updatePayload(&sendPayload, 0, "Message Number 4", 16);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*3)), &sendPayload, payloadSize);
  // Write SHM[4]
  updatePayload(&sendPayload, 0, "", 0);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*4)), &sendPayload, payloadSize);

  // --------------------------------------------------------------------------------
  // Delay, allow other SHM process to read data, write to next 5 blocks of payload memory
  printf("5 Payload messages successfully written to Shared Memory.\n");
  printf("10 sec delay to allow SHM Process 2 read data and next 5 payloads.\n");
  sleep(10);

  // --------------------------------------------------------------------------------
  // Read next 5 payloads from SHM

  // Read SHM[5]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*5)), sizeof(struct Payload));
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[6]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*6)), sizeof(struct Payload));
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[7]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*7)), sizeof(struct Payload));
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[8]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*8)), sizeof(struct Payload));
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[9]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*9)), sizeof(struct Payload));
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  // --------------------------------------------------------------------------------
  // Send 10 payloads to SHM, fill all Shared memory

  // Write SHM[0]
  updatePayload(&sendPayload, 0, "test0", 5);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*0)), &sendPayload, payloadSize); 
  // Write SHM[1]
  updatePayload(&sendPayload, 1, "test1", 5);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*1)), &sendPayload, payloadSize); 
  // Write SHM[2]
  updatePayload(&sendPayload, 0, "test2", 5);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*2)), &sendPayload, payloadSize);
  // Write SHM[3]
  updatePayload(&sendPayload, 1, "test3", 5);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*3)), &sendPayload, payloadSize);
  // Write SHM[4]
  updatePayload(&sendPayload, 0, "test4", 5);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*4)), &sendPayload, payloadSize);
  // Write SHM[5]
  updatePayload(&sendPayload, 1, "test5", 5);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*5)), &sendPayload, payloadSize); 
  // Write SHM[6]
  updatePayload(&sendPayload, 0, "test6", 5);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*6)), &sendPayload, payloadSize); 
  // Write SHM[7]
  updatePayload(&sendPayload, 1, "test7", 5);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*7)), &sendPayload, payloadSize);
  // Write SHM[8]
  updatePayload(&sendPayload, 0, "test8", 5);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*8)), &sendPayload, payloadSize);
  // Write SHM[9]
  updatePayload(&sendPayload, 1, "test9", 5);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  memcpy((shmPtr+(payloadSize*9)), &sendPayload, payloadSize);

  // --------------------------------------------------------------------------------
  // Delay, allow other SHM process to read data, write to next 5 blocks of payload memory
  printf("10 Payload messages successfully written to Shared Memory.\n");
  printf("10 sec delay to allow SHM Process 2 read data and next 5 payloads.\n");
  sleep(10);

  // --------------------------------------------------------------------------------
  // Read 5 payloads from SHM

  // Read SHM[0]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*0)), sizeof(struct Payload));
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[1]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*1)), sizeof(struct Payload));
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[2]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*2)), sizeof(struct Payload));
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[3]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*3)), sizeof(struct Payload));
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  // Read SHM[4]
  memcpy(&rcvPayload, (shmPtr+(payloadSize*4)), sizeof(struct Payload));
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  // --------------------------------------------------------------------------------

  // Cleanup
  fflush(logFile);
  fclose(logFile);
  close(shmFd);
}

