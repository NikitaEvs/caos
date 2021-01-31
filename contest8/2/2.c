#include <unistd.h>    
#include <sys/mman.h>    
#include <sys/types.h>    
#include <sys/stat.h>    
#include <stdio.h>    
#include <stdint.h>    
#include <stdlib.h>
#include <fcntl.h>    
#include <string.h>  

/**
 * Print in the buffer on the position (x; y) with a width W and a size N the number
 * Also print \n after a symbol if x + 1 == N
 * @param buffer
 * @param x
 * @param y
 * @param number
 * @param N
 * @param W
 */
void prettyPrint(char* buffer, int x, int y, int number, int N, int W) {
  /* + y because \n */
  size_t position = (y * N + x) * W + y;
  char temporaryBuffer[W + 1];

  sprintf(temporaryBuffer, "%*d", W, number);
  strncpy(buffer + position, temporaryBuffer, W);

  if ((x + 1) == N) {
    /* Calculate a position for the delimiter */
    position += W;

    buffer[position] = '\n';
  }
}

/**
 * Print a spiral array N x N with a W width for each cell in buffer
 * @param buffer
 * @param N
 * @param W
 */
void sprintSpiral(char* buffer, int N, int W) {
  /**
   * Coordinates system:
   * +------> - x
   * |
   * |
   * v - y
   *
   * Direction value:
   *        ^ - 3
   *        |
   * 2 - <-- --> - 0
   *        |
   *        V - 1
   */
  enum {
    RIGHT,
    DOWN,
    LEFT,
    UP
  };
  int direction = 0;
  const int directionsNumber = 4;
  int currentNumber = 1;
  int currentLength = N - 1;
  int x = 0, y = 0;
  
  while (currentLength > 0) {
    for (int pos = 0; pos < currentLength; ++pos) {
      prettyPrint(buffer, x, y, currentNumber, N, W);

      switch (direction) {
        case RIGHT:
          ++x;
          break;
        case DOWN:
          ++y;
          break;
        case LEFT:
          --x;
          break;
        case UP:
          --y;
          break;
      }

      ++currentNumber;
    }

    direction = (direction + 1) % directionsNumber; 
    if ((direction % directionsNumber) == 0) {
      currentLength -= 2;
      ++y;
      ++x;
    }

    if (currentLength == 0) {
      prettyPrint(buffer, x, y, currentNumber, N, W);
    }
  }
}

int main(int argc, char **argv) {
  const int ERR_RETURN = 1;        
  const int ERR_CODE = -1;        
  const void* MMAP_ERR_CODE = (void *)(-1);        
        
  const int NUMBER_OF_ARGUMENTS = 4;        
  const int FILE_NAME = 1,        
            N_POS = 2,
            W_POS = 3;

  const int BASE = 10;

  char *filename;
  int N, W;

  int fileDescriptor;
  char *buffer;
  size_t bufferSize;

  /* Get input arguments */
  if (argc < NUMBER_OF_ARGUMENTS) {
    return 0;
  }

  filename = argv[FILE_NAME];
  N = strtol(argv[N_POS], NULL, BASE);
  W = strtol(argv[W_POS], NULL, BASE);

  /* Initialize the buffer */
  fileDescriptor = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fileDescriptor == ERR_CODE) {
    goto error;
  }


  bufferSize = (N * N * W + N) * sizeof(char);

  /* Set size of the file */
  if (truncate(filename, bufferSize) == ERR_CODE) {
    goto error;
  }

  /* Map the file */
  buffer = mmap(NULL, bufferSize, PROT_WRITE, MAP_SHARED, fileDescriptor, 0);
  memset(buffer, 0, bufferSize);
  if (buffer == MMAP_ERR_CODE) {
    goto error;
  }

  /* Print a spiral */
  sprintSpiral(buffer, N, W);

  /* Deallocate space */
  if (munmap(buffer, bufferSize) == ERR_CODE) {    
    goto error;    
  }    
    
  if (close(fileDescriptor) == ERR_CODE) {    
    goto error;    
  }   

  return 0;

error:
  perror(NULL);
  return ERR_RETURN;
}
