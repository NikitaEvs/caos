#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t counter = 0;
volatile sig_atomic_t isExit = 0;

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

void SIGINT_handler(int signum) {
  ++counter;
}

void SIGTERM_handler(int signum) {
  isExit = 1;
}

int main() {
  struct sigaction sigintAction;
  struct sigaction sigtermAction;
  memset(&sigintAction, 0, sizeof(sigintAction));
  memset(&sigtermAction, 0, sizeof(sigtermAction));

  sigintAction.sa_handler = SIGINT_handler;
  sigintAction.sa_flags = SA_RESTART;

  sigtermAction.sa_handler = SIGTERM_handler;
  sigtermAction.sa_flags = SA_RESTART;

  abortIfError(sigaction(SIGINT, &sigintAction, NULL));
  abortIfError(sigaction(SIGTERM, &sigtermAction, NULL));

  printf("%d\n", getpid());
  abortIfError(fflush(stdout));


  while (!isExit) {
    pause();
  }

  printf("%d\n", counter);
}

