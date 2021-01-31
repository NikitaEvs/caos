#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

/**
 * Create timestamp from the date in a format day month year
 * @param year
 * @param month
 * @param day
 * @return time_t timestamp in success and (time_t) (-1) in failure
 */
time_t getTimestamp(int year, int month, int day) {
  struct tm timeMark;

  const int yearShift = 1900;
  const int monthShift = 1;

  /* Invalid data format */
  if ((year < 1900) || 
      ((month < 1) || (month > 12)) ||
      ((day < 1) || (day > 31))) {
    return (time_t)(-1);
  }

  /* Fill tm struct with given arguments */
  memset(&timeMark, 0, sizeof(timeMark));

  timeMark.tm_year = year - yearShift;
  timeMark.tm_mon = month - monthShift;
  timeMark.tm_mday = day;
  timeMark.tm_isdst = -1;

  return mktime(&timeMark);
}


int main(int argc, char** argv) {
  const int ERR_RETURN = -1;
  const int ERR_CODE = 1;
  const time_t TIME_ERR_CODE = (time_t)(-1);

  const int NUMBER_OF_ARGUMENTS = 5;
  const int DIR_POSITION = 1,
            YEAR_POSITION = 2,
            MONTH_POSITION = 3,
            DAY_POSITION = 4;

  const int BASE = 10;

  const char* dirName;
  int year, month, day;

  time_t timestamp;

  int directoryFd;
  DIR *directory;
  struct dirent *entry;
  struct stat fileInfo;

  /* Parse the date from command line arguments */ 
  if (argc < NUMBER_OF_ARGUMENTS) {
    return ERR_CODE;
  }

  dirName = argv[DIR_POSITION];
  
  year = strtol(argv[YEAR_POSITION], NULL, BASE);
  month = strtol(argv[MONTH_POSITION], NULL, BASE);
  day = strtol(argv[DAY_POSITION], NULL, BASE);

  timestamp = getTimestamp(year, month, day);
  if (timestamp == TIME_ERR_CODE) {
    perror("Error during parsing date");
    return ERR_CODE;
  }

  /* Open the directory */
  directoryFd = open(dirName, O_RDONLY | O_DIRECTORY);
  directory = fdopendir(directoryFd);
  if (!directory) {
    perror("Error during opening directory");
    return ERR_CODE;
  }

  /* Read entries from the directory */
  while ((entry = readdir(directory))) {
    fstatat(directoryFd, entry->d_name, &fileInfo, 0);

    /* Will print filename if its modification date is later than the timestamp */
    if (fileInfo.st_mtime > timestamp) {
      printf("%s\n", entry->d_name);
    }
  }

  if (closedir(directory) == ERR_RETURN) {
    perror("Error during closing the directory");
    return ERR_CODE;
  }

  return 0;
}

