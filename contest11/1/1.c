#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

volatile sig_atomic_t isExit = 0;
volatile sig_atomic_t value = 0;

/**    
 * Abort program if returnCode equals -1, exit with EXITSTATUS equals 1    
 * @returnCode - code from command to checking    
 */    
void abortIfError(int returnCode) {    
  const int ERR_CODE = -1;    
  const int ERR_RETURN = 1;    
    
  if (returnCode == ERR_CODE) {    
    perror(NULL);    
    exit(ERR_RETURN);    
  }    
}

void SIGUSR1_handler(int signum) {
  ++value;
  printf("%d\n", value);
  abortIfError(fflush(stdout));
}

void SIGUSR2_handler(int signum) {
  value = -value;
  printf("%d\n", value);
  abortIfError(fflush(stdout));
}

void SIGTERM_handler(int signum) {
  isExit = 1;
}


int main() {
  /* Create and bind handlers */
  struct sigaction sigusr1Action = {.sa_handler = SIGUSR1_handler, .sa_flags = SA_RESTART};
  struct sigaction sigusr2Action = {.sa_handler = SIGUSR2_handler, .sa_flags = SA_RESTART};
  struct sigaction sigtermAction = {.sa_handler = SIGTERM_handler, .sa_flags = SA_RESTART};

  abortIfError(sigaction(SIGUSR1, &sigusr1Action, NULL));
  abortIfError(sigaction(SIGUSR2, &sigusr2Action, NULL));
  abortIfError(sigaction(SIGTERM, &sigtermAction, NULL));

  /* Write pid and read initial value */
  printf("%d\n", getpid());
  abortIfError(fflush(stdout));

  scanf("%d", &value);
  abortIfError(fflush(stdin));

  /* Waiting for signals */
  while (!isExit) {
    pause();
  }

  return 0;
}

