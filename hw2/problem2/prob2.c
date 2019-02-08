// ECEN5013
// Date: 2/2/2019
// Author: Brian Ibeling
// About: C program to complete File IO operations to be tracked by 
//        system calls by host and target platforms.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
  FILE* pFile = NULL;
  char* filename = "test_file.txt";
  char* pArray = NULL;
  uint8_t status = 0;
  size_t len = 0;

  printf("*** Starting File IO Test Program ***\n\n");

  // Create file
  creat(filename, O_RDWR);

  // Modify Permissions
  chmod(filename, (S_IRUSR | S_IWUSR));

  // Open the file
  pFile = fopen(filename, "w");

  // Write single char to file
  fputs("a", pFile);

  // Close file
  fclose(pFile);

  // Open file (append mode)
  pFile = fopen(filename, "a");

  // Allocate memory
  pArray = malloc(100);

  // Read input string from user, write to allocated array
  printf("Input string to write to file:\n");
  status = getline(&pArray, &len, stdin);
  if(status <= 0){
    printf("ERROR: Failed to receive input string!\n");
  }

  // Write String to file
  fputs(pArray, pFile);

  // Flush file output
  fflush(pFile);

  // Close file
  fclose(pFile);

  // Open file (reading)
  pFile = fopen(filename, "r");

  // Read a single character
  fgetc(pFile);

  // Read a string of characters
  fgets(pArray, len, pFile);

  // Close file
  fclose(pFile);

  // Free allocated memory
  free((void*)pArray);
}
