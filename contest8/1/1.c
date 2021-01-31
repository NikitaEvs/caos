#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>



struct Item {
  int value;
  uint32_t next_pointer;
};

int main(int argc, char** argv) {
  const int ERR_CODE = -1;
  const int ERR_RETURN = 1;
  const void* MAP_ERR_CODE = (void *)(-1);

  ssize_t INPUT_FILE_COUNT = 2;
  size_t INPUT_FILE = 1;

  char* shift = 0;
  uint32_t currentPosition = 0;

  char* inputFile;

  int fileDescriptor;
  struct stat fileInfo;
  
  /* Read command line arguments */
  if (argc < INPUT_FILE_COUNT) {
    return 0;
  }

  /* Open the file and get a size of it */    
  if ((fileDescriptor = open(argv[INPUT_FILE], O_RDONLY)) == ERR_CODE) {    
    goto error;    
  }    
     
  if (fstat(fileDescriptor, &fileInfo) == ERR_CODE) {    
    goto error;    
  }    
     
  if (fileInfo.st_size == 0) {    
    goto exit;    
  }    

  inputFile = mmap(NULL, fileInfo.st_size, PROT_READ, MAP_PRIVATE, fileDescriptor, 0);
  if (inputFile == MAP_ERR_CODE) {
    goto error;
  }

  /* Iterate over the linked list */
  do {
    struct Item inputItem;
    
    shift = inputFile + currentPosition;
    memcpy(&inputItem, shift, sizeof(inputItem));

    printf("%d\n", inputItem.value);

    currentPosition = inputItem.next_pointer;
  } while (currentPosition != 0);

  if (munmap(inputFile, fileInfo.st_size) == ERR_CODE) {
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

