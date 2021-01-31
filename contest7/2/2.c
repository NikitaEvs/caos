#include <dirent.h>    
#include <errno.h>     
#include <stdlib.h>     
#include <stdio.h>        
#include <sys/types.h>    
#include <sys/stat.h>
#include <string.h>       
#include <stdint.h> 
#include <fcntl.h>
#include <limits.h>    
#include <unistd.h>

/**
 * Check that pattern is a prefix of the text
 * @param pattern
 * @param text
 * @return 1 if pattern is a prefix of the text, 0 otherwise
 */
int isPrefix(const char* pattern, const char* text) {    
  size_t patternSize = strlen(pattern);    
  size_t textSise = strlen(text);    
    
  if (patternSize > textSise) {    
    return 0;    
  } else {    
    return memcmp(pattern, text, patternSize) == 0;    
  }    
}    

/**
 * Recursive function that calculate size of files in the current directory
 * and files in all subdirectories
 * @param path to the directory 
 * @return size of all files or -1 if an error occurs
 */
ssize_t calculateSize(const char *path) {
  const int ERR_CODE = -1;
  const int ERR_RETURN = 1;

  const char *previousDir = "..";
  const char *currentDir = ".";

  int directoryFd;
  DIR *directory;
  struct dirent *entry;
  struct stat fileInfo;

  ssize_t currentSize = 0;

  /* Open the directory */         
  directoryFd = open(path, O_RDONLY | O_DIRECTORY);
  directory = fdopendir(directoryFd);
  if (!directory) {
    perror("Error during opening directory");
    return ERR_CODE;                                                           
  }                                              
                                              
  /* Read entries from the directory */     
  while ((entry = readdir(directory))) {    
    /* Read a directory */
    if (entry->d_type & DT_DIR) {
      /* Check that this is not the previous directory or the current directory */
      if (!isPrefix(previousDir, entry->d_name) && !isPrefix(currentDir, entry->d_name)) {
        char newName[PATH_MAX] = {0};
        ssize_t entrySize;

        strcat(newName, path);
        strcat(newName, "/");
        strcat(newName, entry->d_name);                                       

        entrySize = calculateSize(newName);               
        if (entrySize == ERR_CODE) {
          perror("Error during parsing directories");
          return ERR_CODE;
        } 

        currentSize += entrySize;
      }    
    } else if (entry->d_type & DT_REG) { /* Read a regular file */
      fstatat(directoryFd, entry->d_name, &fileInfo, 0);
      currentSize += fileInfo.st_size;
    }
  }

  if (closedir(directory) == ERR_CODE) {    
    perror("Error during closing the directory");    
    return ERR_CODE;    
  }

  return currentSize;
}

int main(int argc, char** argv) {
  const int ERR_CODE = -1;
  const int ERR_RETURN = 1;
  
  const int NUMBER_OF_ARGUMENTS = 2;
  const int DIR_POSITION = 1;

  const char* dirName;
  ssize_t size; 

  if (argc < NUMBER_OF_ARGUMENTS) {
    return ERR_RETURN;
  }
  dirName = argv[DIR_POSITION];

  size = calculateSize(dirName);
  if (size == ERR_CODE) {
    return ERR_RETURN;
  }

  printf("%ld\n", size);

  return 0;
}

