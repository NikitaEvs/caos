#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * Close all file descriptors that passed as a variadic arguments 
 * @count - numbers of arguments (numbers of a file descriptor)
 * @return -1 if an error occured, 0 otherwise
 */
int closeAll(size_t count, ...) {
  const int ERR_CODE = -1;
  int status = 0;
  va_list args;
  va_start(args, count);

  for (size_t index = 0; index < count; ++index) {
    status = close(va_arg(args, int));
    if (status == ERR_CODE) {
      return ERR_CODE;
    }
  }

  va_end(args);
  return 0;
}

/**
 * Abort program if returnCode equals -1, exit with EXITSTATUS equals 1
 * @returnCode - code from command to checking
 */
void abortIfError(int returnCode) {
  const int ERR_CODE = -1;
  const int ERR_RETURN = 1;

  if (returnCode == ERR_CODE) {
    perror(NULL);
    wait(NULL);
    exit(ERR_RETURN);
  }
}

/**
 * @struct ProcessNode
 * Contains information about the process in pipeline
 *
 * @var ProcessNode::command
 * Member 'command' contains commands that can used to execution the process
 *
 * @var ProcessNode::next
 * Member 'next' contains pointer to the next ProcessNode in the pipeline
 * If there isn't next process in the pipeline, next equals NULL
 *
 * @var ProcessNode::redirectedOut
 * Member 'redirectedOut' contains file descriptor which will be redirected from process
 */
typedef struct ProcessNode {
  char **commands;
  int redirectedOut;
  struct ProcessNode *next;
} ProcessNode;


/**
 * Allocate new node struct
 *
 * @return pointer to the allocated struct, this pointer should be passed to the desctuctor for deletion
 * if the error occured, return NULL
 */
ProcessNode* processNodeContrusctor(char** commands, int redirectedOut, ProcessNode *next) {
  ProcessNode *node = malloc(sizeof(ProcessNode));
  if (!node) {
    return NULL;
  }

  node->commands = commands;
  node->redirectedOut = redirectedOut;
  node->next = next;

  return node;
}

/**
 * Deallocate the node struct
 * This function doesn't worry about deallocation the command field 
 * @node pointer to deallocation, if node equals NULL no operation is performed 
 */ 
void processNodeDestructor(ProcessNode *node) {
  free(node); 
  node = NULL;
}


enum PIPE_FILE_DESCRIPTORS {
  PIPE_READ,
  PIPE_WRITE
};

int executePipeline(ProcessNode *node) {
  const int ERR_CODE = -1;

  const int PIPE_DESCIPTORS_NUMBER = 2;  
  int pipeDescriptors[PIPE_DESCIPTORS_NUMBER];

  const pid_t CHILD_PID = 0;
  pid_t currentPid = -1;

  int subprocessStatus = -1;

  int stdinCopy = -1;
  int stdoutCopy = -1;
  int stderrCopy = -1;

  stdinCopy = dup(STDIN_FILENO);
  stdoutCopy = dup(STDOUT_FILENO);
  stderrCopy = dup(STDERR_FILENO);

  if ((stdinCopy == ERR_CODE) || 
      (stdoutCopy == ERR_CODE) || 
      (stderrCopy == ERR_CODE)) {
    return ERR_CODE; 
  }

  abortIfError(pipe(pipeDescriptors));

  while (node) {
    if ((currentPid = fork()) == CHILD_PID) {
      const int PROGRAM_NAME_POS = 0;

      /* Redirect redirectedOut to the pipe (write descriptor) */
      if (node->next) {
        abortIfError(dup2(pipeDescriptors[PIPE_WRITE], node->redirectedOut));
      }
      closeAll(2, pipeDescriptors[PIPE_WRITE], pipeDescriptors[PIPE_READ]);

      execvp(node->commands[PROGRAM_NAME_POS], node->commands);

      perror(NULL);
      return ERR_CODE;
    }

    closeAll(1, STDIN_FILENO);
  
    /* Redirect STDIN to the pipe (read descriptor) */
    abortIfError(dup2(pipeDescriptors[PIPE_READ], STDIN_FILENO));
    /* Close all descriptors for the pipe */
    closeAll(2, pipeDescriptors[PIPE_READ], pipeDescriptors[PIPE_WRITE]);

    /** 
     * Create new pipe descriptors (now we have two opened pipe, because STDIN_FILENO 
     * references to the previous pipe, but it will be closed)
     */
    abortIfError(pipe(pipeDescriptors));

    node = node->next;

    abortIfError(waitpid(currentPid, &subprocessStatus, 0));
  }

  closeAll(3, pipeDescriptors[PIPE_READ], pipeDescriptors[PIPE_WRITE], STDIN_FILENO);


  abortIfError(dup2(stdinCopy, STDIN_FILENO));
  abortIfError(dup2(stdoutCopy, STDOUT_FILENO));
  abortIfError(dup2(stderrCopy, STDERR_FILENO));
  closeAll(3, stdinCopy, stdoutCopy, stderrCopy);

  return 0;
}

/**
 * Simply add node to the linked list
 */
void addToList(ProcessNode ***list, ProcessNode *node) {
  **list = node;
  *list = &(**list)->next;
}

/**
 * Simply destroy the list
 */
void listDestructor(ProcessNode **list) {
    while (*list) {
      ProcessNode *current = *list;
      *list = (*list)->next;
      processNodeDestructor(current);
    }
}


int main(int argc, char **argv) {
  ProcessNode *errorRoot = NULL;
  ProcessNode *warningRoot = NULL;
  ProcessNode **errorList = &errorRoot;
  ProcessNode **warningList = &warningRoot;

  const int MIN_ARGC = 2;

  const int PROGRAM_NAME_POS = 1;
  char *programName = NULL;

  char *errorGrepProgramArguments[] = { "egrep", "-o", ":[0-9]+:[0-9]+: error", NULL };
  char *warningGrepProgramArguments[] = { "egrep", "-o", ":[0-9]+:[0-9]+: warning", NULL };
  char *cutGrepProgramArguments[] = { "egrep", "-o", ":[0-9]:", NULL };
  char *uniqProgramArguments[] = { "uniq", NULL };
  char *wcProgramArguments[] = { "wc", "-l", NULL };

  if (argc < MIN_ARGC) {
    return 0;
  }

  programName = argv[PROGRAM_NAME_POS];
  char *compilerProgramArguments[] = { "gcc", programName, NULL };

  addToList(&errorList, processNodeContrusctor(compilerProgramArguments, STDERR_FILENO, NULL));
  addToList(&errorList, processNodeContrusctor(errorGrepProgramArguments, STDOUT_FILENO, NULL));
  addToList(&errorList, processNodeContrusctor(cutGrepProgramArguments, STDOUT_FILENO, NULL));
  addToList(&errorList, processNodeContrusctor(uniqProgramArguments, STDOUT_FILENO, NULL));
  addToList(&errorList, processNodeContrusctor(wcProgramArguments, STDOUT_FILENO, NULL));

  abortIfError(executePipeline(errorRoot));

  addToList(&warningList, processNodeContrusctor(compilerProgramArguments, STDERR_FILENO, NULL));
  addToList(&warningList, processNodeContrusctor(warningGrepProgramArguments, STDOUT_FILENO, NULL));
  addToList(&warningList, processNodeContrusctor(cutGrepProgramArguments, STDOUT_FILENO, NULL));
  addToList(&warningList, processNodeContrusctor(uniqProgramArguments, STDOUT_FILENO, NULL));
  addToList(&warningList, processNodeContrusctor(wcProgramArguments, STDOUT_FILENO, NULL));

  abortIfError(executePipeline(warningRoot));

  errorList = &errorRoot;
  warningList = &warningRoot;
  listDestructor(errorList);
  listDestructor(warningList);
  
  return 0;
}

