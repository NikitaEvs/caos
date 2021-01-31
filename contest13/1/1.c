#define _GNU_SOURCE
#include <ctype.h>
#include <errno.h>
#include <stdio.h>    
#include <stdlib.h>    
#include <stdint.h>    
#include <string.h>     
#include <signal.h>    
#include <sys/epoll.h>
#include <sys/types.h>       
#include <sys/socket.h>    
#include <sys/signalfd.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
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

enum ARGUMENTS_INFO {
   ARGUMENTS_NUMBER = 2,
   SERVICE_POS = 1,
   BUFFER_SIZE = 4096
};

typedef struct {
  int descriptor;
  char *buffer;
  size_t pos;
  size_t size;
  int allBytesRead;
} Client;

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

int serverStart(int sockDescriptor) {
  const int MAX_CONNECTION_NUMBER = 1000;
  const int ERR_CODE = -1;

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
    const int MAX_EVENT_NUMBER = 50; 
    const int DISABLE_TIMEOUT = -1;
    struct epoll_event inputEvents[MAX_EVENT_NUMBER];
    int connectionDescriptor = -1;

    int eventsNumber = abortIfError(epoll_wait(epollDescriptor,
                                    inputEvents,
                                    MAX_EVENT_NUMBER,
                                    DISABLE_TIMEOUT));
    int isExit = 0;
    
    for (int pos = 0; pos < eventsNumber; ++pos) {
      if (inputEvents[pos].data.fd == signalDescriptor) {
        isExit = 1;
        break;
      } else if (inputEvents[pos].data.fd == sockDescriptor) { 
        connectionDescriptor = abortIfError(accept4(sockDescriptor, NULL, NULL, SOCK_NONBLOCK)); 
        
        Client *client = malloc(sizeof(Client));
        if (!client) return ERR_CODE;

        client->descriptor = connectionDescriptor;
        client->buffer = malloc(BUFFER_SIZE * sizeof(char));
        client->allBytesRead = 0;
        client->pos = 0;
        client->size = 0;
        
        abortIfError(epoll_ctl(epollDescriptor, 
                               EPOLL_CTL_ADD, 
                               connectionDescriptor,
                               &(struct epoll_event)
                               {.events = EPOLLIN, .data = {.ptr = client}}));
      } else {
        Client *currentClient = inputEvents[pos].data.ptr;
        if (inputEvents[pos].events & EPOLLIN) {
          ssize_t readBytes = read(currentClient->descriptor,
                                   currentClient->buffer,
                                   BUFFER_SIZE);
          currentClient->size = readBytes;

          if (readBytes > 0) {
            for (ssize_t pos = 0; pos < readBytes; ++pos) {
              currentClient->buffer[pos] = toupper(currentClient->buffer[pos]);
            }

            abortIfError(epoll_ctl(epollDescriptor, 
                                   EPOLL_CTL_MOD,
                                   currentClient->descriptor,
                                   &(struct epoll_event)
                                   {.events = EPOLLOUT, .data = {.ptr = currentClient}}));
          } else {
            abortIfError(epoll_ctl(epollDescriptor,
                                   EPOLL_CTL_DEL,
                                   currentClient->descriptor,
                                   NULL));
            free(currentClient->buffer);
            free(currentClient);

            shutdown(currentClient->descriptor, SHUT_RDWR);
            close(currentClient->descriptor);
          }
        } 
        if (inputEvents[pos].events & EPOLLOUT) {
          ssize_t writeBytes = write(currentClient->descriptor,
                                     currentClient->buffer + currentClient->pos,
                                     currentClient->size - currentClient->pos);
          if (writeBytes > 0) {
            currentClient->pos += writeBytes;
            if (currentClient->pos == currentClient->size) {
              currentClient->pos = 0;
              currentClient->size = 0;
              abortIfError(epoll_ctl(epollDescriptor, 
                                     EPOLL_CTL_MOD,
                                     currentClient->descriptor,
                                     &(struct epoll_event)
                                     {.events = EPOLLIN, .data = {.ptr = currentClient}}));
            }
          } else {
            abortIfError(epoll_ctl(epollDescriptor,
                                   EPOLL_CTL_DEL,
                                   currentClient->descriptor,
                                   NULL));
            shutdown(currentClient->descriptor, SHUT_RDWR);
            close(currentClient->descriptor);

            free(currentClient->buffer);
            free(currentClient);
          }
        }
      }
    }

    if (isExit) break;
  }

  close(epollDescriptor);
  shutdown(sockDescriptor, SHUT_RDWR);
  close(sockDescriptor);

  return 0;
}


int main(int argc, char **argv) {
  if (argc < ARGUMENTS_NUMBER) return 0;
  
  int serverDescriptor = serverConstructor("localhost", argv[SERVICE_POS]);
  abortIfError(serverStart(serverDescriptor));

  return 0;
}

