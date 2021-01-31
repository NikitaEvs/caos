#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
  const int ERR_RETURN = 1;
  const int MAX_SIZE = 4096;

  const char* programName = "python3";
  const char* flag = "-c";

  char expression[MAX_SIZE];
  char buffer[MAX_SIZE];

  char *expressionEnd;
  char inputDelimiter = '\n';
  char expressionDelimiter = '\0';

  if (!fgets(expression, sizeof(expression), stdin)) {
    return ERR_RETURN;
  }

  expressionEnd = strchr(expression, inputDelimiter);
  if (expressionEnd) {
    *expressionEnd = expressionDelimiter;
  }

  if (strlen(expression) == 0) {
    return 0;
  }
 
  if (snprintf(buffer, sizeof(buffer), "print(%s)", expression) < 0) {
    return ERR_RETURN;
  }

  execlp(programName, programName, flag, buffer, NULL);

  perror(NULL);
  return ERR_RETURN;
}
