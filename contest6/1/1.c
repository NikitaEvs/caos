#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>


const int32_t ERR_CODE = -1;

struct Item {
  int value;
  uint32_t next_pointer;
};

int main(int argc, char** argv) {
  ssize_t INPUT_FILE_COUNT = 2;
  size_t INPUT_FILE = 1;
  ssize_t shiftStatus = 0;
  ssize_t readStatus = 0;
  uint32_t currentPosition = 0;
  int inputFile = 0;
  int32_t returnCode = 0;
  
  if (argc < INPUT_FILE_COUNT) {
    return 0;
  }

  inputFile = open(argv[INPUT_FILE], O_RDONLY);
  if (inputFile == ERR_CODE) {
    return ERR_CODE;
  }

  do {
    struct Item inputItem;
    
    shiftStatus = lseek(inputFile, currentPosition, SEEK_SET);   
    if (shiftStatus == ERR_CODE) {
      returnCode = ERR_CODE;
      goto exit;
    }

    readStatus = read(inputFile, &inputItem, sizeof(inputItem));
    if (readStatus == ERR_CODE || readStatus == 0) {
      returnCode = ERR_CODE;
      goto exit;
    }

    printf("%d\n", inputItem.value);

    currentPosition = inputItem.next_pointer;
  } while (currentPosition != 0);


exit:
  close(inputFile);    
  return returnCode;
}

