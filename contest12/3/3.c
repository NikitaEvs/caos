#include <errno.h>
#include <stdio.h>    
#include <stdlib.h>    
#include <string.h>     
#include <signal.h>    
#include <sys/epoll.h>
#include <sys/types.h>       
#include <sys/socket.h>    
#include <sys/signalfd.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/wait.h>
#include <fcntl.h>
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

/**
 * Construct server, create socket and bind an address
 * @return file descriptor to the server or break the program if an error occured
 */
int serverConstructor(char *addr, char *service) {
  struct addrinfo *entry;
  struct addrinfo hints;
  int sockDescriptor = -1;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET; // IPv4
  hints.ai_socktype = SOCK_STREAM; // TCP
  abortIfError(getaddrinfo(addr, service, &hints, &entry));

  sockDescriptor = abortIfError(socket(entry->ai_family,
                                       entry->ai_socktype,
                                       /* socket protocol: auro */ 0));
  
  abortIfError(bind(sockDescriptor, entry->ai_addr, entry->ai_addrlen));

  freeaddrinfo(entry);
  return sockDescriptor;
}

int serverStart(int sockDescriptor, void (*handler)(int)) {
  const int MAX_CONNECTION_NUMBER = 50;

  int signalDescriptor = -1;
  int epollDescriptor = -1;
  struct epoll_event sockEvent = {.events = EPOLLIN | EPOLLOUT, .data = {.fd = sockDescriptor}};
  struct epoll_event signalEvent;
  
  /* Create a mask for blocking all signals excluding SIGTERM and SIGINT */
  sigset_t signalsMask;
  sigset_t allBlocked;
  abortIfError(sigemptyset(&signalsMask));
  abortIfError(sigfillset(&allBlocked));
  abortIfError(sigaddset(&signalsMask, SIGTERM));
  abortIfError(sigaddset(&signalsMask, SIGINT));

  /* Create signalfd for a sync signals processing 
   * and block all input signals using sigprocmask 
   */
  abortIfError(sigprocmask(SIG_SETMASK, &allBlocked, NULL));
  signalDescriptor = abortIfError(signalfd(signalDescriptor, &signalsMask, 0));
  signalEvent.events = EPOLLIN | EPOLLOUT;
  {
    epoll_data_t data;
    data.fd = signalDescriptor;
    signalEvent.data = data;
  }

  /* Set up epoll for sync processing */
  epollDescriptor = abortIfError(epoll_create1(0));
  abortIfError(epoll_ctl(epollDescriptor, EPOLL_CTL_ADD, sockDescriptor, &sockEvent));
  abortIfError(epoll_ctl(epollDescriptor, EPOLL_CTL_ADD, signalDescriptor, &signalEvent));

  /* Start listening */
  abortIfError(listen(sockDescriptor, MAX_CONNECTION_NUMBER));

  while (1) {
    /* We can create epoll with all connection to clients too, but
     * it's necessary to handle only one connection if signal occured, so we cannot do it
     */
    const int MAX_EVENT_NUMBER = 1; 
    const int DISABLE_TIMEOUT = -1;
    struct epoll_event inputEvent;
    int connectionDescriptor = -1;

    /* And we don't care about the number from epoll_wait (if it doesn't equal -1)
     * because we set indefinitely block on waiting and MAX_EVENT_NUMBER equals 1, so
     * result of this function can be only 1
     */
    abortIfError(epoll_wait(epollDescriptor,
                            &inputEvent,
                            MAX_EVENT_NUMBER,
                            DISABLE_TIMEOUT));
    
    if (inputEvent.data.fd == signalDescriptor) {
      break;
    }

    /* We don't care about an address of the client */ 
    connectionDescriptor = abortIfError(accept(sockDescriptor, NULL, NULL)); 
      
    /* It can be blocking handler because signals will be handled by epoll using signalfd */
    handler(connectionDescriptor);

    shutdown(connectionDescriptor, SHUT_RDWR);
    close(connectionDescriptor);
  }

  close(epollDescriptor);
  shutdown(sockDescriptor, SHUT_RDWR);
  close(sockDescriptor);

  return 0;
}

/**    
* Utility function for safely checking that one c-style string is a suffix for another    
* @param input: the first array for comparing    
* @param pos: start position for comparing    
* @param maxSize: size of first c-style string      
* @param pattern: the second array    
* @param patternSize: size of the second array      
* @return 1 if one c-style is a suffix to another    
*/
int safeCompare(const char *input, size_t pos, size_t maxSize,    
                                    const char* pattern, size_t patternSize) {    
  char selectedBlock[patternSize + 1];    
  selectedBlock[patternSize] = 0;    
  const char* shiftInput = input + pos;    
  strncpy(selectedBlock, shiftInput, patternSize);    
  return (pos + patternSize <= maxSize) &&    
      (strcmp(selectedBlock, pattern) == 0);    
}    

enum ARGUMENTS_INFO {
   ARGUMENTS_NUMBER = 3,
   SERVICE_POS = 1,
   PATH_POS = 2,
   BUFFER_SIZE = 4096
};
char *basedir = NULL;

void requestHandler(int clientDescriptor) {
  static char buffer[BUFFER_SIZE] = {};
  static char filename[BUFFER_SIZE] = {};
  const char *delimiter = "\r\n";
  size_t delimiterSize = strlen(delimiter);

  size_t sumReadBytes = 0;

  /* Parse request header */
  while (1) {
    ssize_t readBytes = read(clientDescriptor, buffer + sumReadBytes, BUFFER_SIZE - sumReadBytes);
    if (readBytes == 0) break;

    abortIfError(readBytes);
    sumReadBytes += readBytes;

    if ((sumReadBytes > delimiterSize) && 
        safeCompare(buffer, sumReadBytes - delimiterSize, BUFFER_SIZE,
                    delimiter, delimiterSize)) {
        break;
    }
  }

  if (sscanf(buffer, "GET %s HTTP/1.1", filename) == EOF) {
    return; 
  }

  /* Process request */
  {
    const char *fmtString = "HTTP/1.1 %s\r\nContent-Length: %d\r\n\r\n";
    const char *executableFmtString = "HTTP/1.1 %s\r\n";
    const char *SUCCESS_HEADER = "200 OK";
    const char *NOT_FOUND_HEADER = "404 Not Found";
    const char *FORBIDDEN_HEADER = "403 Forbidden";

    const char *status = SUCCESS_HEADER;
    int isOK = 1;
    int isExecutable = 1;

    const int ERR_CODE = -1;

    int directoryDescriptor = abortIfError(open(basedir, O_DIRECTORY | O_RDONLY));
    int fileDescriptor = openat(directoryDescriptor, filename, O_RDONLY);

    static char headerBuffer[BUFFER_SIZE] = {};
    ssize_t headerBufferSize = 0;

    ssize_t alreadyWritten = 0;
    ssize_t currentWritten = 0;

    off_t fileSize = 0;
    struct stat fileInfo;

    if (fileDescriptor == ERR_CODE) {
      if (errno == EACCES) {
        status = FORBIDDEN_HEADER;
        isOK = 0;
      } else {
        status = NOT_FOUND_HEADER;
        isOK = 0;
      }
    }
   
    if (isOK) {
      abortIfError(fstat(fileDescriptor, &fileInfo));
      fileSize = fileInfo.st_size;
      isExecutable = (fileInfo.st_mode & S_IXUSR);
    }

    if (isExecutable) {
      if (snprintf(headerBuffer, BUFFER_SIZE, executableFmtString, status) < 0) {
        abortIfError(ERR_CODE);
      }
    } else {
      if (snprintf(headerBuffer, BUFFER_SIZE, fmtString, status, fileSize) < 0) {
        abortIfError(ERR_CODE);
      }
    }

    headerBufferSize = strlen(headerBuffer);
    
    while (alreadyWritten < headerBufferSize) {
      currentWritten = write(clientDescriptor, 
                             headerBuffer + alreadyWritten, 
                             headerBufferSize - alreadyWritten);
      abortIfError(currentWritten);
      alreadyWritten += currentWritten;
    }

    if (isOK) {
      if (isExecutable) {
        pid_t CHILD_PID = 0;
        pid_t currentPid = CHILD_PID;

        /* I can do it because all signals wait in the signalfd */
        currentPid = abortIfError(fork());
        if (currentPid == CHILD_PID) {
          abortIfError(dup2(clientDescriptor, STDOUT_FILENO));
          close(clientDescriptor);
          close(fileDescriptor);
          close(directoryDescriptor);

          abortIfError(chdir(basedir));
          execl(filename, filename, NULL);
          abortIfError(ERR_CODE);
        }
        abortIfError(waitpid(currentPid, NULL, 0));
      } else {
        abortIfError(sendfile(clientDescriptor, fileDescriptor, NULL, fileSize));
      }
    }
    
    close(fileDescriptor);
    close(directoryDescriptor);
  }
}


int main(int argc, char **argv) {
  if (argc < ARGUMENTS_NUMBER) return 0;
  basedir = argv[PATH_POS];
  
  int serverDescriptor = serverConstructor("localhost", argv[SERVICE_POS]);
  abortIfError(serverStart(serverDescriptor, requestHandler));

  return 0;
}

