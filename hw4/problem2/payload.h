/* File for common objects and definitions shared across various files */

#include <time.h>
#include <sys/time.h>

/*
enum ledCmd {
  LEDOFF = 0,
  LEDON
};
*/

struct Payload {
  unsigned char cmd;
  char msg[50];
  unsigned char length;
};


void updatePayload(struct Payload* payload, unsigned char cmd, char* msg, unsigned char length){
  payload->cmd = cmd;
  strcpy(payload->msg, msg);
  payload->length = length;
}

/* Method to return a string containing the current timestamp.
 * Also removes newline char added to end of string
 */
char* getTimestamp() {
  struct timeval tv;
  char* timestamp;

  gettimeofday(&tv, NULL);
  timestamp = asctime(gmtime(&tv.tv_sec));
  timestamp[24] = '\0'; // Done to remove newline char added to end of string
  return timestamp;
}
