#include <signal.h>
#include <sys/types.h>
#include <sys/signalfd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


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

enum {
  BUFFER_SIZE = 4096 * 1024
};

int main(int argc, char **argv) {
  const int ERR_CODE = -1;
  const int ERR_RETURN = 1;
  const int MIN_ARGUMENTS_COUNT = 2;

  int sigFileDescriptor = -1;

  static char buffer[BUFFER_SIZE]; 

  sigset_t blockedSignals;
  sigset_t handledSignals;

  struct signalfd_siginfo sigInfo = {};

  if (argc < MIN_ARGUMENTS_COUNT) {
    return 0;
  }

  FILE *currentFiles[argc - 1];

  for (int pos = 1; pos < argc; ++pos) {
    currentFiles[pos - 1] = fopen(argv[pos], "r");
    if (currentFiles[pos - 1] == NULL) {
      goto error;
    }
  }

  abortIfError(sigemptyset(&handledSignals));
  /* Set handler for all signals from SIGRTMIN to SIGRTMAX */
  for (int sigNumber = 0; sigNumber < argc; ++sigNumber) {
    abortIfError(sigaddset(&handledSignals, SIGRTMIN + sigNumber));
  }

  abortIfError(sigfillset(&blockedSignals));
  abortIfError(sigprocmask(SIG_BLOCK, &blockedSignals, NULL));

  sigFileDescriptor = signalfd(sigFileDescriptor, &handledSignals, 0);
  if (sigFileDescriptor == ERR_CODE) {
    goto error;
  }

  while (1) {
    int position = -1;

    abortIfError(read(sigFileDescriptor, &sigInfo, sizeof(sigInfo)));
    position = sigInfo.ssi_signo - SIGRTMIN;

    if (position == 0) {
      break;
    } 

    if (memset(buffer, 0, sizeof(buffer)) == NULL) {
      goto error;
    } 

    if (fgets(buffer, sizeof(buffer), currentFiles[position - 1]) != NULL) {
      fputs(buffer, stdout);
      abortIfError(fflush(stdout));
    }
  }
  
  close(sigFileDescriptor);
  for (int pos = 0; pos < argc - 1; ++pos) {
    fclose(currentFiles[pos]);
  }

  return 0;

error:
  return ERR_RETURN;
}

