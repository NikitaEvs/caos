#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

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

void SIGRTMIN_handler(int sig, siginfo_t *info, void *ucontext) {
  unsigned int N = (info->si_value).sival_int;

  if (N == 0) {
    isExit = 1;
  } else {
    union sigval value;
    --N;
    value.sival_int = N;

    abortIfError(sigqueue(info->si_pid, sig, value));
  }
}


int main() {
  sigset_t allBlock;
  abortIfError(sigfillset(&allBlock));

  struct sigaction sigAction = {.sa_sigaction = SIGRTMIN_handler, 
                                .sa_mask = allBlock, 
                                .sa_flags = SA_RESTART | SA_SIGINFO};
  
  sigset_t blockedSignals;
  abortIfError(sigfillset(&blockedSignals));
  abortIfError(sigdelset(&blockedSignals, SIGRTMIN));
  abortIfError(sigprocmask(SIG_SETMASK, &blockedSignals, NULL));

  abortIfError(sigaction(SIGRTMIN, &sigAction, NULL));

  while (!isExit) {
    pause();
  }

  return 0;
}

