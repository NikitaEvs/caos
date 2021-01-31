#define _FILE_OFFSET_BITS 64
#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>


const int ERR_CODE = -1;

int proccessFile(char* name) {
  const char linkPrefix[] = "link_to_";
  char pathBuffer[PATH_MAX];

  struct stat fileInfo = {};

  if (lstat(name, &fileInfo) == ERR_CODE) {
    return ERR_CODE;
  }

  if (S_ISLNK(fileInfo.st_mode)) {
    realpath(name, pathBuffer);
    printf("%s\n", pathBuffer);
  } else if (S_ISREG(fileInfo.st_mode)) {
    strcat(pathBuffer, linkPrefix);
    strcat(pathBuffer, basename(name));

    symlink(name, pathBuffer);
  }
  
  return 0;
}

int main() {
  const char stringDelimiter = '\n';
  const char stringEnd = '\0';

  char buffer[PATH_MAX];
  char* endOfStringPtr;

  while (fgets(buffer, sizeof(buffer), stdin)) {
    endOfStringPtr = memchr(buffer, stringDelimiter, sizeof(buffer));
    if (endOfStringPtr) {
      *endOfStringPtr = stringEnd;
    }
    
    if (proccessFile(buffer) == ERR_CODE) {
      return 1;
    }
  } 

  return 0;
}
