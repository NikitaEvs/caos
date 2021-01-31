#define _FILE_OFFSET_BITS 64

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>


const int ERR_CODE = -1;

off_t calculateRegularFile(const char* name) {
  struct stat fileInfo;

  int statusCode = lstat(name, &fileInfo);
  if (statusCode == ERR_CODE) {
    return ERR_CODE;
  }

  if (S_ISREG(fileInfo.st_mode)) {
    return fileInfo.st_size;
  } else {
    return 0;
  }
}

int main() {
  const char stringDelimiter = '\n';
  const char stringEnd = '\0';

  long long unsigned int size = 0;
  char buffer[PATH_MAX];
  char* endOfStringPtr;

  while (fgets(buffer, sizeof(buffer), stdin)) {
    endOfStringPtr = memchr(buffer, stringDelimiter, sizeof(buffer));
    if (endOfStringPtr) {
      *endOfStringPtr = stringEnd;
    }

    off_t currentSize = calculateRegularFile(buffer);
    if (currentSize > 0) {
      size += currentSize;
    }
  } 

  printf("%llu\n", size);

  return 0;
}

