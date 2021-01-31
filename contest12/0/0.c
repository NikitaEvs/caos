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
  MIN_ARGUMENTS_NUMBER = 3,
  IPv4_ADDR_POS = 1,
  PORT_POS = 2
};

int main(int argc, char **argv) {
  const int ERR_CODE = -1;

  char *addr = NULL;
  char *service = NULL;

  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  
  int readStdinNumber = -1;
  int readSockNumber = -1;

  struct addrinfo *entries = NULL;
  int sockDescriptor = 0;
  int numberCounter = 0;

  if (argc < MIN_ARGUMENTS_NUMBER) return 0;
  addr = argv[IPv4_ADDR_POS];
  service = argv[PORT_POS];

  /* Get addr info */
  hints.ai_family = AF_INET; // IPv4
  hints.ai_socktype = SOCK_STREAM; // TCP
  abortIfError(getaddrinfo(addr, service, &hints, &entries));

  /* Create endpoint */
  /* It seems like an overhead but it's more convinient solution in case when 
   * we use not localhost but another hostname 
   */
  do {
    sockDescriptor = abortIfError(socket(entries->ai_family, 
                                  entries->ai_socktype, 
                                  /* socket protocol: auto */ 0));

    if (connect(sockDescriptor, entries->ai_addr, entries->ai_addrlen) != ERR_CODE) {
      break;
    }

    close(sockDescriptor);
  } while ((entries = entries->ai_next));

  if ((sockDescriptor == ERR_CODE) || (!entries)) {
    abortIfError(ERR_CODE);
  }

  /* Parse input stream and send it to the socket (using flag MSG_NOSIGNAL because we can get unexpected 
   * closing by the server
   */
  while (scanf("%d", &readStdinNumber) > 0) {
    ++numberCounter;
    if (send(sockDescriptor, &readStdinNumber, sizeof(readStdinNumber), MSG_NOSIGNAL) == ERR_CODE) {
      break;
    }
  }
  
  /* Read numberCounter numbers from the socket and print it using whitespace as a delimiter */
  while (numberCounter && (read(sockDescriptor, &readSockNumber, sizeof(readSockNumber)))) {
    --numberCounter;

    printf("%d ", readSockNumber);
  }


  /* Deallocate memory */
  freeaddrinfo(entries);
  shutdown(sockDescriptor, SHUT_RDWR);
  close(sockDescriptor);
  entries = NULL;

  return 0;
}

