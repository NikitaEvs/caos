#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/** This function prints all numbers from 1 to N in the stdout with whitespace as a delimiter
 *  using multiply processes
 *  @param N - maximum number for printing
 *  @return -1 if some error happened, 0 otherwise
 */
int printNumberChain(long unsigned int N) {
  const long unsigned int MIN_NUMBER = 1;
  const int ERR_CODE = -1;
  const int CHILD_PID = 0;

  pid_t pid;
  long unsigned int counter = 1;

  struct rlimit limits;

  if (getrlimit(RLIMIT_NPROC, &limits) == ERR_CODE) {
    return ERR_CODE;
  }

  if ((N < MIN_NUMBER) || (N > limits.rlim_cur)) {
    return ERR_CODE;
  }


  while (counter <= N) {
    if (counter > 1) {
      putchar(' ');
    }

    printf("%lu", counter);
    if (fflush(stdout) == EOF) {
      perror(NULL);
      return ERR_CODE;
    }
    
    
    if (counter < N) {
      pid = fork();

      if (pid > CHILD_PID) {
        const int ERR_RETURN = 1;
        int status = 0;

        if (waitpid(pid, &status, 0) == ERR_CODE) {
          return ERR_CODE;
        }

        if (WEXITSTATUS(status) == ERR_RETURN) {
          return ERR_CODE;
        }

        break;
      }
    }

    ++counter;
  } 

  if ((counter == 1) || (N == 1)) {
    printf("\n");
  } 

  return 0;
}

int main(int argc, char **argv) {
  const int ERR_CODE = -1;
  const int ERR_RETURN = 1;

  const int ARGUMENT_COUNT = 2;
  const int N_POSITION = 1;
  const int N_BASE = 10;

  long unsigned int N = 0;

  if (argc < ARGUMENT_COUNT) {
    return 0;
  }

  N = strtoul(argv[N_POSITION], NULL, N_BASE);

  if (printNumberChain(N) == ERR_CODE) {
    return ERR_RETURN;
  }

  return 0;
}

