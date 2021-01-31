#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>

enum ARGUMENTS_POS {
  CMD_POS = 1,
  IN_POS = 2
};

enum PIPES_DESCRIPTORS {
  READ,
  WRITE
};

int main(int argc, char **argv) {
  const int ERR_CODE = -1;
  const int ERR_RETURN = 1;
  const int CHILD_PID = 0;

  const int NUMBER_OF_ARGUMENTS = 3;

  char *cmd = NULL;
  char *in = NULL;

  int inFileDescriptor = -1;

  const int pipeDescriptorsNumber = 2;
  int pipeFileDescriptors[pipeDescriptorsNumber];

  pid_t forkPid = -1;
  int childStatus = -1;

  ssize_t bytesCounter = 0;
  ssize_t readBytes = 0;

  const size_t BUFFER_SIZE = 4096;
  char buffer[BUFFER_SIZE];

  if (argc < NUMBER_OF_ARGUMENTS) {
    return 0;
  } 

  cmd = argv[CMD_POS];
  in = argv[IN_POS];


  if ((inFileDescriptor = open(in, O_RDONLY)) == ERR_CODE) {
    goto error;
  }

  if (dup2(inFileDescriptor, STDIN_FILENO) == ERR_CODE) {
    goto error;
  }

  if (close(inFileDescriptor) == ERR_CODE) {
    goto error;
  }

  if (pipe(pipeFileDescriptors) == ERR_CODE) {
    goto error;
  }

  forkPid = fork();
  if (forkPid == CHILD_PID) {
    if (close(pipeFileDescriptors[READ]) == ERR_CODE) {
      goto error;
    }

    if (dup2(pipeFileDescriptors[WRITE], STDOUT_FILENO) == ERR_CODE) {
      goto error;
    }

    if (close(pipeFileDescriptors[WRITE]) == ERR_CODE) {
      goto error;
    }
    
    execlp(cmd, cmd, NULL);
    goto error;
  }

  if (close(pipeFileDescriptors[WRITE]) == ERR_CODE) {
    goto error;
  }

  while ((readBytes = read(pipeFileDescriptors[READ], buffer, sizeof(buffer)))) {
    if (readBytes == ERR_CODE) {
      goto error;
    }

    bytesCounter += readBytes;
  } 

  if (waitpid(forkPid, &childStatus, 0) == ERR_CODE) {
    goto error;
  }

  if (WEXITSTATUS(childStatus) == ERR_RETURN) {
    goto error;
  }

  
  printf("%lu\n", bytesCounter);
  
  
  return 0;

error:
  perror(NULL);
  close(pipeFileDescriptors[WRITE]);
  close(pipeFileDescriptors[READ]);
  return ERR_RETURN;
}

