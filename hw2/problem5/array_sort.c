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
#include <unistd.h>
#include <errno.h>

#define ARRAY_SIZE 256
#define MYSORT_SYSCALL_NUM 398

//---------------------------------------------------------------------------
// Function Prototypes
void printArray(int32_t* buffer, size_t len);

//---------------------------------------------------------------------------
int main() {
  int32_t unsortedArray[ARRAY_SIZE];
  int32_t sortedArray[ARRAY_SIZE];
  unsigned long status;
  size_t i;

  // Populate unsortedArray with random values
  srand(time(NULL));
  for(i=0; i<ARRAY_SIZE; i++) {
    unsortedArray[i] = rand();
  }

  // Sort array using the defined syscall sys_prob5sort()
  printf("\n\nUsing Kernel Sort syscall() to sort the following array with size {%d} [User Space] :\n", ARRAY_SIZE);
  printArray(unsortedArray, ARRAY_SIZE);
  sleep(3); // sleep added to allow serial output log to clear
  status = syscall(MYSORT_SYSCALL_NUM, unsortedArray, sortedArray, (int)ARRAY_SIZE);
  sleep(3); // sleep added to allow serial output log to clear
  printf("Kernel Sort syscall() returned with status {%ld}:\n", status);
  printf("\n\nKernel Sort syscall() has sorted the following array [User Space] :\n");
  printArray(sortedArray, ARRAY_SIZE);

  // Now test array sort syscall() with invalid parameters
  printf("\n\n ** Invalid input parameter tests **\n\n");
  status = syscall(MYSORT_SYSCALL_NUM, NULL, sortedArray, (int)ARRAY_SIZE);
  printf("*TEST 1*\nKernel Sort syscall() with NULL input returned with status {%ld}:\n", status);
  sleep(2);

  status = syscall(MYSORT_SYSCALL_NUM, unsortedArray, NULL, (int)ARRAY_SIZE);
  printf("\n*TEST 2*\nKernel Sort syscall() with NULL input returned with status {%ld}:\n", status);
  sleep(2);

  status = syscall(MYSORT_SYSCALL_NUM, unsortedArray, sortedArray, (int)0);
  printf("\n*TEST 3\nKernel Sort syscall() with 0 length returned with status {%ld}:\n", status);
  sleep(2);

  status = syscall(MYSORT_SYSCALL_NUM, unsortedArray, sortedArray, (int)-1);
  printf("\n*TEST 4\nKernel Sort syscall() with -1 length returned with status {%ld}:\n", status);
  sleep(2);

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
