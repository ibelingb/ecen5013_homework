/* ECEN5013 - HW4
 * Date: 2/28/2019
 * Author: Brian Ibeling
 * About: C program to demonstrate multithreading and sharing data between
 *        multiple threads via a file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> /* For file handling */
#include <pthread.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/time.h>
//#include <sys/types.h>
//#include <sys/wait.h>

#define NUM_THREADS 3
#define WT 5
#define CPU_STAT_LINES 2
#define STRING_BUFFER_SIZE 500

/* Define static and global variables */
pthread_t gThreads[NUM_THREADS];
pthread_mutex_t fileMutex;

struct threadInfo
{
  pid_t pid;
  char* filename;
};

/* Define module parameters */
static void *parentHandler(void* tInfo);
static void *childHandler1(void* tInfo);
static void *childHandler2(void* tInfo);

/* ------------------------------------------------------------- */
int main(void) {
  char* filename = "shared_file.txt";
  int status = 0;
  struct threadInfo tInfo;

  // TODO: Update to accept filename by cmdline

  // Create log file; Populate threadInfo object
  creat(filename, O_RDWR);
  tInfo.pid = getpid();
  tInfo.filename = filename;

  // Create parent thread
  printf("Creating parent thread.\n");
  status = pthread_create(&gThreads[0], NULL, parentHandler, (void*)&tInfo);
  if(status) {
    printf("ERROR: Failed to create parentThread - exiting main().\n");
    return -1;
  }

  // Initialize parent thread
  pthread_join(gThreads[0], NULL);
  printf("Main() process complete.\n");

  return 0;
}

/* ------------------------------------------------------------- */
static void *parentHandler(void* tInfo) {
  struct threadInfo parentTinfo = *(struct threadInfo*)tInfo;
  FILE* pFile = NULL;
  struct timeval tv;
  int status = 0;

  // Capture thread start time
  gettimeofday(&tv, NULL);

  // Open shared file to write to
  if ((pFile = fopen(parentTinfo.filename, "a")) == NULL){
    printf("ERROR: Failed to open logfile for parentThread- terminating.\n");
    return NULL;
  }

  // Write thread info and starttime to file
  pthread_mutex_lock(&fileMutex);
  fprintf(pFile, "[Parent Thread]: Started at %s", asctime(gmtime(&tv.tv_sec)));
  fprintf(pFile, "[Parent Thread]: Started with PID {%d} and TID {%d}.\n", 
          parentTinfo.pid, (pid_t)syscall(SYS_gettid));
  pthread_mutex_unlock(&fileMutex);

  // Create child thread 1
  printf("Creating child thread 1.\n");
  status = pthread_create(&gThreads[1], NULL, childHandler1, (void*)&parentTinfo);
  if(status) {
    printf("ERROR: Failed to create childThread1 - terminating.\n");
    return NULL;
  }

  // Create child thread 2
  printf("Creating child thread 2.\n");
  status = pthread_create(&gThreads[2], NULL, childHandler2, (void*)&parentTinfo);
  if(status) {
    printf("ERROR: Failed to create childThread2 - terminating.\n");
    return NULL;
  }

  // Initialize child threads; wait until all threads are finished
  pthread_join(gThreads[1], NULL);
  pthread_join(gThreads[2], NULL);

  // Write parent thread complete to log file
  gettimeofday(&tv, NULL);
  pthread_mutex_lock(&fileMutex);
  fprintf(pFile, "[Parent Thread]: Completed at {%s}\n", asctime(gmtime(&tv.tv_sec)));
  pthread_mutex_unlock(&fileMutex);

  // Close shared file
  fclose(pFile);

  printf("Parent thread complete.\n");

  return NULL;
}

/* ------------------------------------------------------------- */
static void *childHandler1(void* tInfo) {
  struct threadInfo child1Tinfo = *(struct threadInfo*)tInfo;
  FILE* pFile = NULL;
  struct timeval tv;
  //int status = 0;

  // Capture thread start time
  gettimeofday(&tv, NULL);

  // Open shared file to write to
  if ((pFile = fopen(child1Tinfo.filename, "a")) == NULL){
    printf("ERROR: Failed to open logfile for childThread1 - terminating.\n");
    return NULL;
  }

  // Write thread info and starttime to file
  pthread_mutex_lock(&fileMutex);
  fprintf(pFile, "[Child Thread 1]: Started at %s", asctime(gmtime(&tv.tv_sec)));
  fprintf(pFile, "[Child Thread 1]: Started with PID {%d} and TID {%d}.\n", 
          child1Tinfo.pid, (pid_t)syscall(SYS_gettid));
  pthread_mutex_unlock(&fileMutex);

  // Read input file to analyze; print an error if file not found or fails to open
  // TODO

  sleep(WT);

  // Close shared file
  fclose(pFile);

  printf("Child thread 1 complete.\n");
  return NULL;
}

/* ------------------------------------------------------------- */
static void *childHandler2(void* tInfo) {
  struct threadInfo child2Tinfo = *(struct threadInfo*)tInfo;
  FILE* pFile = NULL;
  FILE* pCpuFile = NULL;
  char* fileOutput;
  char* cpuStatus[CPU_STAT_LINES];
  struct timeval tv;
  size_t i = 0;
  size_t len = 0;
  //int status = 0;

  // Capture thread start time
  gettimeofday(&tv, NULL);

  // Open shared file to write to
  if ((pFile = fopen(child2Tinfo.filename, "a")) == NULL){
    printf("ERROR: Failed to open logfile for childThread2 - terminating.\n");
    return NULL;
  }

  // Write thread info and starttime to file
  pthread_mutex_lock(&fileMutex);
  fprintf(pFile, "[Child Thread 2]: Started at %s", asctime(gmtime(&tv.tv_sec)));
  fprintf(pFile, "[Child Thread 2]: Started with PID {%d} and TID {%d}.\n", 
          child2Tinfo.pid, (pid_t)syscall(SYS_gettid));
  pthread_mutex_unlock(&fileMutex);

  // Allocate memory for file read buffer
  for(i=0; i<CPU_STAT_LINES; i++){
    cpuStatus[i] = malloc(STRING_BUFFER_SIZE*sizeof(char));
  }

  int count = 0;

  // Enter a while loop to print CPU utilization every 100 msec
  while(count < 6) {
    // Open /proc/stat file to read from
    if ((pCpuFile = fopen("/proc/stat", "r")) == NULL){
      fclose(pFile);
      printf("ERROR: Failed to open /proc/stat for childThread2 - terminating.\n");
      return NULL;
    }

    // Read first lines from /proc/stat file
    for(i=0; i<CPU_STAT_LINES; i++){
      getline(&fileOutput, &len, pCpuFile);
      strcpy(cpuStatus[i], fileOutput);
    }

    pthread_mutex_lock(&fileMutex);
    for(i=0; i<CPU_STAT_LINES; i++){
      fprintf(pFile, "[Child Thread 2]: /proc/stat line[%d]:%s", i, cpuStatus[i]);
    }
    pthread_mutex_unlock(&fileMutex);

    // Close /proc/stat file
    fclose(pCpuFile);

    sleep(1);
    count++;
  }

  // Close shared file and /proc/stat file
  fclose(pFile);

  printf("Child thread 2 complete.\n");
  return NULL;
}

/* ------------------------------------------------------------- */
