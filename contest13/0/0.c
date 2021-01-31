#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


enum SIZES {
  BUFFER_SZIE = 4096,
  MAX_EVENTS_NUMBER = 10000
};

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
 * Make all file descriptors that passed as a variadic arguments non-blocking 
 * @count - numbers of arguments (numbers of a file descriptor)
 * @return -1 if an error occured, 0 otherwise
 */
int makeNonBlock(size_t count, ...) {
  const int ERR_CODE = -1;
  int flags = 0;
  va_list args;
  va_start(args, count);

  for (size_t index = 0; index < count; ++index) {
    int currentFd = va_arg(args, int); 
    flags = fcntl(currentFd, F_GETFL);
    flags = fcntl(currentFd, F_SETFL, flags | O_NONBLOCK);

    if (flags == ERR_CODE) {
      return ERR_CODE;
    }
  }

  va_end(args);
  return 0;
}

/**
 * Struct for a file entry in the epoll queue 
 * @param descriptor specifies file descriptor for the current open file
 * @param bytesRead is amount of read data (in bytes)
 */
typedef struct {
  int descriptor;
  size_t bytesRead;
} EpollFile;

/**
 * Constructor for EpollFile struct
 * @epollDescriptor of the linked epoll queue
 * @fileDescriptor specify descriptor of the current EpollFile (will be NON_BLOCK)
 * @return pointer to the allocated struct, or NULL if an error occured
 * Note: this function can abort program if epoll_ctl produces error
 */
EpollFile *EpollFileConstructor(int epollDescriptor, int fileDescriptor) {
  abortIfError(makeNonBlock(1, fileDescriptor));  

  EpollFile *newFile = malloc(sizeof(EpollFile));
  if (!newFile) return NULL;

  newFile->descriptor = fileDescriptor;
  newFile->bytesRead = 0;

  abortIfError(epoll_ctl(epollDescriptor, 
                         EPOLL_CTL_ADD, 
                         fileDescriptor, 
                         &(struct epoll_event)
                         {.events = EPOLLIN, .data.ptr = newFile}));

  return newFile;
}

void EpollFileDestructor(EpollFile *epollFile) {
  free(epollFile);
}

/**
 * Process epollFile event: read bytes from nonblock descriptor and store number of read bytes
 * @return 1 if there is no data to read, or 0 otherwise
 */
int EpollFileProcessing(EpollFile *epollFile) {
  static char buffer[BUFFER_SZIE] = {};
  const int READ_BYTES_STATUS = 0;
  const int NO_READ_STATUS = 1;

  ssize_t readBytes = read(epollFile->descriptor, buffer, BUFFER_SZIE);
  if (readBytes > 0) {
    epollFile->bytesRead += readBytes;
    return READ_BYTES_STATUS;
  } else if (readBytes == 0) {
    close(epollFile->descriptor);
    return NO_READ_STATUS;
  } else {
    const int ERR_CODE = -1;
    abortIfError(ERR_CODE);
    return -1;
  }
}



extern size_t read_data_and_count(size_t N, int in[N]) {
  int ERR_CODE = -1;
  int epollDescriptor = -1;
  size_t readBytes = 0;
  size_t processedFiles = N;

  EpollFile **epollFiles = NULL;

  struct epoll_event events[MAX_EVENTS_NUMBER];

  epollFiles = malloc(N * sizeof(*epollFiles));
  if (!epollFiles) abortIfError(ERR_CODE);

  epollDescriptor = abortIfError(epoll_create1(0));

  for (size_t numPos = 0; numPos < N; ++numPos) {
    epollFiles[numPos] = EpollFileConstructor(epollDescriptor, in[numPos]);
    if (!epollFiles[numPos]) abortIfError(ERR_CODE);
  }


  while (processedFiles > 0) {
    int eventsNumber = abortIfError(epoll_wait(epollDescriptor, 
                                               events, 
                                               MAX_EVENTS_NUMBER, 
                                               /* no timeout */ -1));
    for (int eventPos = 0; eventPos < eventsNumber; ++eventPos) {
      if (events[eventPos].events & EPOLLIN) {
        processedFiles -= abortIfError(EpollFileProcessing(events[eventPos].data.ptr));
      }
    }
  }

  close(epollDescriptor);

  for (size_t epollFilePos = 0; epollFilePos < N; ++epollFilePos) {
    readBytes += epollFiles[epollFilePos]->bytesRead;
    EpollFileDestructor(epollFiles[epollFilePos]);
  }
  free(epollFiles);

  return readBytes;
}

