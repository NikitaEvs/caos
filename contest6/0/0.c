#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>


const int32_t ERR_CODE = -1;
const int32_t INPUT_FILE_DOES_NOT_EXIST = 1;
const int32_t OUTPUT_FILE_DOES_NOT_EXIST = 2;
const int32_t READ_ERROR = 3; 

ssize_t bufferizedRead(int fdInput, char** buffer) {
  const size_t BUFFER_SIZE = 4096;

  size_t bufferCurrentSize = BUFFER_SIZE;
  ssize_t symbolsCurrentCount = 0;
  ssize_t readingStatus = 0;

  *buffer = realloc(*buffer, bufferCurrentSize);
  
  while ((readingStatus = read(fdInput, *buffer, BUFFER_SIZE)) > 0) {
    symbolsCurrentCount += readingStatus;

    if (symbolsCurrentCount + BUFFER_SIZE >= bufferCurrentSize) {
      bufferCurrentSize += BUFFER_SIZE;
      *buffer = realloc(*buffer, bufferCurrentSize);
    }
  }

  if (readingStatus == ERR_CODE) {
    return ERR_CODE;
  } else {
    return symbolsCurrentCount;
  }
}

int32_t closeAll(size_t count, ...) {
  int32_t status = 0;
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


int main(int argc, char** argv) {
  const int32_t INPUT_FILE_COUNT = 4; 
  const int32_t INPUT_FILE = 1;
  const int32_t DIGIT_OUTPUT_FILE = 2;
  const int32_t NONDIGIT_OUTPUT_FILE = 3;
  char* buffer = NULL;
  char* digitBuffer = NULL;
  size_t digitBufferPtr = 0;
  size_t nonDigitBufferPtr = 0;
  char* nonDigitBuffer = NULL;
  int inputFile = 0; 
  int digitFile = 0;
  int nonDigitFile = 0;
  ssize_t fileSize = 0;
  ssize_t writeReturnCode = 0;
  int returnCode = 0;

  if (argc < INPUT_FILE_COUNT) {
    return READ_ERROR; 
  }

  inputFile = open(argv[INPUT_FILE], O_RDONLY);
  if (inputFile == ERR_CODE) {
    return INPUT_FILE_DOES_NOT_EXIST;
  }

  digitFile = open(argv[DIGIT_OUTPUT_FILE], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
  if (digitFile == ERR_CODE) {
    closeAll(1, inputFile);

    return OUTPUT_FILE_DOES_NOT_EXIST;
  }

  nonDigitFile = open(argv[NONDIGIT_OUTPUT_FILE], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
  if (nonDigitFile == ERR_CODE) {
    closeAll(2, inputFile, digitFile);

    return OUTPUT_FILE_DOES_NOT_EXIST;
  }

  fileSize = bufferizedRead(inputFile, &buffer);
  if (fileSize == ERR_CODE) {
    closeAll(3, inputFile, digitFile, nonDigitFile);
    free(buffer);

    return READ_ERROR;
  }

  digitBuffer = malloc(fileSize * sizeof(char));
  nonDigitBuffer = malloc(fileSize * sizeof(char));
  for (ssize_t pos = 0; pos < fileSize; ++pos) {
    if (isdigit(buffer[pos])) {
      *(digitBuffer + digitBufferPtr) = *(buffer + pos);
      ++digitBufferPtr;
    } else {
      *(nonDigitBuffer + nonDigitBufferPtr) = *(buffer + pos);
      ++nonDigitBufferPtr;
    }
  }
  writeReturnCode = write(digitFile, digitBuffer, digitBufferPtr);
  if (writeReturnCode == ERR_CODE) {
    returnCode = READ_ERROR;
    goto exit;
  }

  writeReturnCode = write(nonDigitFile, nonDigitBuffer, nonDigitBufferPtr);
  if (writeReturnCode == ERR_CODE) {
    returnCode = READ_ERROR;
    goto exit;
  }

exit:
  closeAll(3, inputFile, digitFile, nonDigitFile);
  free(buffer);
  free(digitBuffer);
  free(nonDigitBuffer);
  return returnCode;
}

