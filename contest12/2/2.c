#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>   
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

/**    
 * Abort program if returnCode equals -1, exit with EXITSTATUS equals 1    
 * @returnCode - code from command to checking    
 */    
int abortIfError(int returnCode) {    
  const int ERR_CODE = -1;    
  const int ERR_RETURN = 1;    
    
  if (returnCode == ERR_CODE) {    
    perror(NULL);    
    exit(ERR_RETURN);    
  }    

  return returnCode;
}

enum ARGUMENTS_INFO {
  MIN_ARGUMENTS_NUMBER = 2,
  PORT_POS = 1
};

int main(int argc, char **argv) {
  const int ERR_CODE = -1;

  const char *addr = "localhost";
  char *service = NULL;

  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  
  int readStdinNumber = -1;
  int readSockNumber = -1;

  struct addrinfo *entry = NULL;
  int sockDescriptor = 0;
  int numberCounter = 0;

  if (argc < MIN_ARGUMENTS_NUMBER) return 0;
  service = argv[PORT_POS];

  /* Get addr info */
  hints.ai_family = AF_INET; // IPv4
  hints.ai_socktype = SOCK_DGRAM; // UDP 
  abortIfError(getaddrinfo(addr, service, &hints, &entry));

  /* Create endpoint */
  sockDescriptor = abortIfError(socket(entry->ai_family, 
                                entry->ai_socktype, 
                                /* socket protocol: auto */ 0));

  /* Parse input stream and send it to the socket (using flag MSG_NOSIGNAL because we can get unexpected 
   * closing by the server)
   */
  while (scanf("%d", &readStdinNumber) > 0) {
    ++numberCounter;
    if (sendto(sockDescriptor, &readStdinNumber, sizeof(readStdinNumber), MSG_NOSIGNAL,
               entry->ai_addr, entry->ai_addrlen) == ERR_CODE) {
      break;
    }
  }
  
  /* Read numberCounter numbers from the socket and print it using whitespace as a delimiter */
  while (numberCounter && (read(sockDescriptor, &readSockNumber, sizeof(readSockNumber)))) {
    --numberCounter;

    printf("%d ", readSockNumber);
  }


  /* Deallocate memory */
  freeaddrinfo(entry);
  close(sockDescriptor);
  entry = NULL;

  return 0;
}

