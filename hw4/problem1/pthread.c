/* ECEN5013 - HW4
 * Date: 2/28/2019
 * Author: Brian Ibeling
 * About: C program to demonstrate multithreading and sharing data between
 *        multiple threads via a file.
 * Resources: Utilized the following resourecs when developing the code contained in this file.
 *    - https://riptutorial.com/posix/example/16306/posix-timer-with-sigev-thread-notification
 *    - https://www3.physnet.uni-hamburg.de/physnet/Tru64-Unix/HTML/APS33DTE/DOCU_007.HTM
 *    - http://devarea.com/linux-handling-signals-in-a-multithreaded-application/#.XHNsfuhKiHt
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> /* For file handling */
#include <pthread.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/signal.h>
//#include <sys/types.h>
//#include <sys/wait.h>

#define NUM_THREADS 3
#define WT 5
#define CPU_STAT_LINES 2
#define READ_BUFFER_SIZE 100

/* Define static and global variables */
pthread_t gThreads[NUM_THREADS];
pthread_mutex_t gFileMutex;
char* gLogFilename;

struct threadInfo
{
  pid_t pid;
  char* filename;
};

/* Define Function Prototypes */
static void *parentHandler(void* tInfo);
static void *childHandler1(void* tInfo);
static void *childHandler2(void* tInfo);
static void cpuTimerHandler(union sigval sv);
void sigHandler(int signo);

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
  pthread_mutex_lock(&gFileMutex);
  fprintf(pFile, "[Parent Thread]: Started at %s", asctime(gmtime(&tv.tv_sec)));
  fprintf(pFile, "[Parent Thread]: Started with PID {%d} and TID {%d}.\n", 
          parentTinfo.pid, (pid_t)syscall(SYS_gettid));
  pthread_mutex_unlock(&gFileMutex);

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
  pthread_mutex_lock(&gFileMutex);
  fprintf(pFile, "[Parent Thread]: Completed at {%s}\n", asctime(gmtime(&tv.tv_sec)));
  pthread_mutex_unlock(&gFileMutex);

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
  pthread_mutex_lock(&gFileMutex);
  fprintf(pFile, "[Child Thread 1]: Started at %s", asctime(gmtime(&tv.tv_sec)));
  fprintf(pFile, "[Child Thread 1]: Started with PID {%d} and TID {%d}.\n", 
          child1Tinfo.pid, (pid_t)syscall(SYS_gettid));
  pthread_mutex_unlock(&gFileMutex);

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
  struct timeval tv;
  struct sigevent sigEvent;
  struct itimerspec timeSpec;
  timer_t timerId;
  int status = 0;

  // Define signal handlers and global var for log filename
  gLogFilename = child2Tinfo.filename;
  signal(SIGUSR1, sigHandler);

  // Clear struct memory to 0
  memset(&tv, 0, sizeof(struct timeval));
  memset(&sigEvent, 0, sizeof(struct sigevent));
  memset(&timeSpec, 0, sizeof(struct itimerspec));

  // Populate variables needed for timer
  sigEvent.sigev_notify = SIGEV_THREAD;
  sigEvent.sigev_value.sival_ptr = child2Tinfo.filename;
  sigEvent.sigev_notify_function = cpuTimerHandler;
  sigEvent.sigev_notify_attributes = NULL;
  // Define time to trigger every 100 msec
  timeSpec.it_value.tv_sec = 0;
  timeSpec.it_value.tv_nsec = 100000000;
  timeSpec.it_interval.tv_sec = 0;
  timeSpec.it_interval.tv_nsec = 100000000;

  // Capture thread start time
  gettimeofday(&tv, NULL);

  // Open shared file to write to
  if ((pFile = fopen(child2Tinfo.filename, "a")) == NULL){
    printf("ERROR: Failed to open logfile for childThread2 - terminating.\n");
    return NULL;
  }

  // Write thread info and starttime to file
  pthread_mutex_lock(&gFileMutex);
  fprintf(pFile, "[Child Thread 2]: Started at %s", asctime(gmtime(&tv.tv_sec)));
  fprintf(pFile, "[Child Thread 2]: Started with PID {%d} and TID {%d}.\n", 
          child2Tinfo.pid, (pid_t)syscall(SYS_gettid));
  pthread_mutex_unlock(&gFileMutex);

  // Create and set timer
  status = timer_create(CLOCK_REALTIME, &sigEvent, &timerId);
  if(status != 0){
    printf("Failed to create Posix timer for childThread2 - terminating.\n");
    return NULL;
  }
  status = timer_settime(timerId, 0, &timeSpec, NULL);
  if(status != 0){
    printf("Failed to start Posix timer for childThread2 - terminating.\n");
    return NULL;
  }

  // Loop program - Sig Event Handler will kill thread
  sleep(10);
  
  // Close shared file
  fclose(pFile);

  printf("Child thread 2 complete.\n");
  return NULL;
}

/* ------------------------------------------------------------- */
static void cpuTimerHandler(union sigval sv){
  FILE* pFile;
  FILE* pCpuFile = NULL;
  char* filename = (char*)sv.sival_ptr;
  char cpuStatus1[READ_BUFFER_SIZE];
  char cpuStatus2[READ_BUFFER_SIZE];

  // Open /proc/stat file to read from
  if ((pCpuFile = fopen("/proc/stat", "r")) == NULL){
    printf("ERROR: Failed to open /proc/stat for childThread2 - returning from timer thread.\n");
    return;
  }

  // Open shared file to write to
  if ((pFile = fopen(filename, "a")) == NULL){
    printf("ERROR: Failed to open logfile for childThread2 - terminating.\n");
    return;
  }

  // Read lines from /proc/stat file
  // Read data before taking mutex to limit time in critical section
  fgets(cpuStatus1, sizeof(cpuStatus1), pCpuFile);
  fgets(cpuStatus2, sizeof(cpuStatus2), pCpuFile);

  // Write output to file
  pthread_mutex_lock(&gFileMutex);
  fprintf(pFile, "[Child Thread 2]:\n%s%s", cpuStatus1, cpuStatus2);
  pthread_mutex_unlock(&gFileMutex);

  // Close /proc/stat file
  fclose(pFile);
  fclose(pCpuFile);
}

/* ------------------------------------------------------------- */
void sigHandler(int signo) {
  FILE * pLogFile = NULL;
  if((pLogFile = fopen(gLogFilename, "a")) == NULL){
    printf("ERROR: Failed to open logfile for sigHandler - terminating.\n");
    return;
  }

  if(signo == SIGUSR1){
    pthread_mutex_lock(&gFileMutex);
    fprintf(pLogFile, "USR1 Signal received");
    pthread_mutex_unlock(&gFileMutex);

  }

}
/* ------------------------------------------------------------- */
