#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


enum {
  BUFFER_SIZE = 4096
};

int main() {
  const int ERR_CODE = 1;
  const int ERR_RETURN = 1;
  const int CHILD_PID = 0;

  const char *programCode = "#include <stdio.h>\n"
                            "int main() {\n"
                            "  printf(\"%%d\\n\", (%s));\n"
                            "  return 0;\n"
                            "}\n\0";

  const char *compilerName = "cc";
  const char *compilerArg = "-o";

  static char buffer[BUFFER_SIZE] = {};
  char fileBuffer[BUFFER_SIZE];

  ssize_t bytesRead = 0;
  size_t  bytesAlreadyRead = 0;
  
  char tempFilename[] = "/tmp/calcXXXXXX.c";
  const int suffixLength = 2;
  char tempExecutableName[] = "/tmp/execXXXXXX";
  int tempFileDescriptor;
  int tempExecutableDescriptor;

  long unsigned int writtenBytes = 0;

  pid_t compilerPid;
  pid_t binaryPid;
  int status;

  while ((bytesRead = read(0, buffer + bytesAlreadyRead, BUFFER_SIZE - bytesAlreadyRead)) > 0) {
    bytesAlreadyRead += bytesRead;
  }

  if (bytesRead == ERR_CODE) {
    goto error;
  }

  if (sprintf(fileBuffer, programCode, buffer) <= ERR_CODE) {
    goto error;
  } 

  tempFileDescriptor = mkstemps(tempFilename, suffixLength);
  if (tempFileDescriptor == ERR_CODE) {
    goto error;
  }

  while (writtenBytes < strlen(fileBuffer)) {
    int additionalWrittenBytes = write(tempFileDescriptor, fileBuffer, strlen(fileBuffer)); 
    if (additionalWrittenBytes == ERR_CODE) {
      goto error;
    }

    writtenBytes += additionalWrittenBytes;
  }

  tempExecutableDescriptor = mkostemp(tempExecutableName, 0);
  if (tempExecutableDescriptor == ERR_CODE) {
    goto error;
  }

  compilerPid = fork();
  if (compilerPid == CHILD_PID) {
    execlp(compilerName, compilerName, tempFilename, compilerArg, tempExecutableName, NULL);
    goto error;
  }
  
  if (waitpid(compilerPid, &status, 0) == ERR_CODE) {
    goto error;
  }

  if (close(tempFileDescriptor) == ERR_CODE) {
    goto error;
  }

  if (close(tempExecutableDescriptor) == ERR_CODE) {
    goto error;
  }

  binaryPid = fork();
  if (binaryPid == CHILD_PID) {
    execl(tempExecutableName, tempExecutableName, NULL);
    goto error;
  }

  if (waitpid(binaryPid, &status, 0) == ERR_CODE) {
    goto error;
  }

  return 0;

error:
  perror(NULL);
  return ERR_RETURN;
}
