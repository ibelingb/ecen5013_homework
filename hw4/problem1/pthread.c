/* ECEN5013 - HW4
 * Date: 2/28/2019
 * Author: Brian Ibeling
 * About: C program to demonstrate multithreading and sharing data between
 *        multiple threads via a file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
//#include <sys/types.h>
//#include <sys/wait.h>

#define NUM_THREADS 2
#define WT 5

/* Define static and global variables */
pthread_t mThreads[NUM_THREADS];

struct threadInfo
{
  pthread_t tid;
  FILE * pFile;
};


/* Define module parameters */
static void *childHandler1();
static void *childHandler2();

/* ------------------------------------------------------------- */
int main(void) {
  FILE* pFile = NULL;
  char* filename = "shared_file.txt";
  int status = 0;

  // Output info
  printf("mainThread has started with PID {%d} and TID {%d}.\n", getpid(), (pid_t)syscall(SYS_gettid));

  // Create child thread 1
  printf("Creating child thread 1.\n");
  status = pthread_create(&mThreads[0], NULL, childHandler1, (void*)NULL);
  if(status) {
    printf("ERROR: Failed to create childThread1 - terminating.\n");
    return -1;
  }

  // Create child thread 2
  printf("Creating child thread 2.\n");
  status = pthread_create(&mThreads[1], NULL, childHandler2, (void*)NULL);
  if(status) {
    printf("ERROR: Failed to create childThread2 - terminating.\n");
    return -1;
  }

  // Initialize child threads 
  pthread_join(mThreads[0], NULL);
  pthread_join(mThreads[1], NULL);

  sleep(WT*2);
  printf("Killing parent thread.\n");

  return 0;
}

/* ------------------------------------------------------------- */

/* ------------------------------------------------------------- */
static void *childHandler1() {
  int status = 0;

  printf("childHandler1 has started with PID {%d} and TID {%d}.\n", getpid(), (pid_t)syscall(SYS_gettid));

  sleep(WT);
  printf("Killing child1 thread.\n");
  return NULL;
}

/* ------------------------------------------------------------- */
static void *childHandler2() {
  int status = 0;

  printf("childHandler2 has started with PID {%d} and TID {%d}.\n", getpid(), (pid_t)syscall(SYS_gettid));

  sleep(WT);
  printf("Killing child2 thread.\n");
  return NULL;
}
