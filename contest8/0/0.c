#define _GNU_SOURCE

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char** argv) {
  const int ERR_RETURN = 1;    
  const int ERR_CODE = -11;    
  const void* MMAP_ERR_CODE = (void *)(-1);    
    
  const int NUMBER_OF_ARGUMENTS = 3;    
  const int TEXT_NAME = 1,    
            PATTERN_NAME = 2;    

  char *filename;
  char *pattern;
  char *buffer;
  char *ptr;

  int fileDescriptor;

  struct stat fileInfo;
    
  /* Parse the date from command line arguments */     
  if (argc < NUMBER_OF_ARGUMENTS) {    
    goto error;
  }    

  filename = argv[TEXT_NAME];
  pattern = argv[PATTERN_NAME];

  /* Open the file and get a size of it */
  if ((fileDescriptor = open(filename, O_RDONLY)) == ERR_CODE) {
    goto error;
  }

  if (fstat(fileDescriptor, &fileInfo) == ERR_CODE) {
    goto error;
  }

  if (fileInfo.st_size == 0) {
    goto exit;
  }

  /* Mapping memory */
  buffer = mmap(NULL, fileInfo.st_size, PROT_READ, MAP_PRIVATE, fileDescriptor, 0);
  if (buffer == MMAP_ERR_CODE) {
    goto error;
  }

  ptr = buffer;

  
  /* Read file and find occurences */
  while (NULL != (ptr = strstr(ptr, pattern))) {
    size_t distance = ptr - buffer;
    printf("%lu ", distance);
    
    ++ptr;
  }
  printf("\n");

  if (munmap(buffer, fileInfo.st_size) == ERR_CODE) {
    goto error;
  }

exit:

  if (close(fileDescriptor) == ERR_CODE) {
    goto error;
  }

  return 0;

error:
  perror(NULL);
  return ERR_RETURN;
}

