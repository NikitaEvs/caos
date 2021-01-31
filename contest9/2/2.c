#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/** This function calculates number of words in buffer (each word without whitespaces)
 *  @return -1 if some error happened, 0 
 */
int calculateWords() {
  const int ERR_CODE = -1;
  const int CHILD_PID = 0;
  
  const int BUFFER_SIZE = 4096;

  pid_t pid;
  int counter = 0;
  int bytesCount = 0;

  do {
    char buffer[BUFFER_SIZE];

    bytesCount = scanf("%s", buffer);
    if (fflush(stdin) == EOF) {
      perror(NULL);
      return ERR_CODE;
    }

    if (bytesCount == EOF) {
      return 0;
    }

    pid = fork();
  } while ((bytesCount != EOF) && (pid == CHILD_PID));

  if (pid != CHILD_PID) {
    int status = 0;
    int exitStatus = 0;

    waitpid(pid, &status, 0);
    exitStatus = WEXITSTATUS(status);

    return exitStatus + 1;
  } else {
    return 1;
  }

  return counter;
}

int main() {
  const int ERR_CODE = -1;
  const int ERR_RETURN = 1;

  pid_t originalPID = getpid();

  int wordNumber = calculateWords();
  if (wordNumber == ERR_CODE) {
    return ERR_RETURN;
  }

  if (getpid() == originalPID) {
    printf("%d\n", wordNumber);
    return 0;
  } else {
    return wordNumber;
  }
}

