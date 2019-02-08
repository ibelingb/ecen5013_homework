// array_sort.c
// ECEN5013
// Date: 2/3/2019
// Author: Brian Ibeling
// About: Array sort method (largest to smallest) to demonstrating running in Kernel Space

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define ARRAY_SIZE 10 // initial testing
//#define ARRAY_SIZE 256
#define SYSCALL_NUM 398

//---------------------------------------------------------------------------
// Function Prototypes
void printArray(int32_t* buffer, size_t len);
int sortArrayLargest(int32_t* inputBuffer, int32_t* outputBuffer, size_t len);

//---------------------------------------------------------------------------
int main() {
  int32_t unsortedArray[ARRAY_SIZE];
  int32_t sortedArray[ARRAY_SIZE];
  long status;
  size_t i;

  // Populate unsortedArray with random values
  srand(time(NULL));
  for(i=0; i<ARRAY_SIZE; i++) {
    unsortedArray[i] = rand();
  }

  // Sort array using the defined syscall sys_prob5sort()
  sortArrayLargest(unsortedArray, sortedArray, (size_t)ARRAY_SIZE);

  return 0;
}

//---------------------------------------------------------------------------
int sortArrayLargest(int32_t* inputBuffer, int32_t* outputBuffer, size_t len) {
  size_t i, j = 0;
  int32_t* sortedArray = NULL;

  // Validate received inputs
  if((inputBuffer == NULL) || (outputBuffer == NULL) | (len <= 0)){
    printf("Invalid inputs received. sortArray failed to complete successfully.\n");
    return -1; // TODO: update to use errno.h
  }

  printf("Beginning sortedArray() for received array of length {%ld}.\nReceived array contents:\n", len);
  printArray(inputBuffer, len);

  // Allocate memory
  sortedArray = malloc(len*sizeof(int32_t));
  if(sortedArray == NULL){
    printf("Failed to allocate memory for sorting array.\n");
    return -1; // TODO - update
  }

  // Copy input array contents to output array
  memcpy(sortedArray, inputBuffer, len*sizeof(int32_t));

  // Copy array into kernel space (Pre-sort)
  // TODO

  // Sort array
  int32_t swapValue = 0;
  for(i=0; i<(len-1); i++){
    for(j=0; j<(len-i-1); j++){
      if(sortedArray[j] < sortedArray[j+1]){
        swapValue = sortedArray[j];
        sortedArray[j] = sortedArray[j+1];
        sortedArray[j+1] = swapValue;
      }
    }
  }


  // Copy array into user space (Post-sort)
  // TODO

  // Print sorted array contents
  printf("\nSorted Array:\n");
  printArray(sortedArray, len);

  // Deallocate memory
  free(sortedArray);
  return 0;
}

//---------------------------------------------------------------------------
void printArray(int32_t* buffer, size_t len){
  size_t i = 0;
  for(i=0; i<len; i++) {
    printf("Buffer[%ld]: %d\n", i, buffer[i]);
  }
}

//---------------------------------------------------------------------------
