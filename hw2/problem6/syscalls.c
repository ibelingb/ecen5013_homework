// array_sort.c
// ECEN5013 - HW2, prob5
// Date: 2/3/2019
// Author: Brian Ibeling
// About: Array sort method (largest to smallest) to demonstrating running in Kernel Space

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>

#define ARRAY_SIZE 10
#define MYSORT_SYSCALL_NUM 398
#define PID_SYSCALL_NUM 20 // Process Syscall ID
#define UID_SYSCALL_NUM 24 // User Syscall ID
#define DT_SYSCALL_NUM 13  // Date/Time Syscall ID

//---------------------------------------------------------------------------
// Function Prototypes
void printArray(int32_t* buffer, size_t len);

//---------------------------------------------------------------------------
int main() {
  int32_t unsortedArray[ARRAY_SIZE];
  int32_t sortedArray[ARRAY_SIZE];
  unsigned long status;
  size_t i;
  struct timeval tv;

  // Populate unsortedArray with random values
  srand(time(NULL));
  for(i=0; i<ARRAY_SIZE; i++) {
    unsortedArray[i] = rand();
  }

  // Sort array using the defined syscall sys_prob5sort()
  printf("\n\nUsing Kernel Sort syscall() to sort the following array with size {%d} [User Space] :\n", ARRAY_SIZE);
  printArray(unsortedArray, ARRAY_SIZE);
  sleep(1); // sleep added to allow serial output log to clear
  status = syscall(MYSORT_SYSCALL_NUM, unsortedArray, sortedArray, (int)ARRAY_SIZE);
  sleep(1); // sleep added to allow serial output log to clear
  printf("Kernel Sort syscall() returned with status {%ld}:\n", status);
  printf("\n\nKernel Sort syscall() has sorted the following array [User Space] :\n");
  printArray(sortedArray, ARRAY_SIZE);

  // Make Syscall for PID
  status = syscall(PID_SYSCALL_NUM);
  printf("\nReturned PID: {%ld}\n", status);

  // Make Syscall for UID
  status = syscall(UID_SYSCALL_NUM);
  printf("Returned UID: {%ld}\n", status);

  // Make Syscall for Date/Time
  gettimeofday(&tv, NULL);
  printf("Returned Date/Time: %s\n", asctime(gmtime(&tv.tv_sec)));

  return 0;
}

//---------------------------------------------------------------------------
// Helper Functions
//---------------------------------------------------------------------------
void printArray(int32_t* buffer, size_t len){
  size_t i = 0;
  for(i=0; i<len; i++) {
    printf("Buffer[%ld]: %d\n", i, buffer[i]);
  }
}

//---------------------------------------------------------------------------
