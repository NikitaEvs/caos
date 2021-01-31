#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

/**
 * Represent date
 * Using as an argument in getTimestamp
 */
struct date {
  int year;
  int month;
  int day;
  int hour;
  int minute;
};

/**
 * Create timestamp from the date in a format day month year
 * @param date : struct with a date data
 * @return time_t timestamp in success and (time_t) (-1) in failure
 */
time_t getTimestamp(const struct date *date) {
  struct tm timeMark;

  const int yearShift = 1900;
  const int monthShift = 1;

  /* Invalid data format */
  if ((date->year < 1900) || 
      ((date->month < 1) || (date->month > 12)) ||
      ((date->day < 1) || (date->day > 31)) ||
      ((date->hour < 0) || (date->hour > 23)) ||
      ((date->minute < 0) || (date->minute > 59))) {
    return (time_t)(-1);
  }

  /* Fill tm struct with given arguments */
  memset(&timeMark, 0, sizeof(timeMark));

  timeMark.tm_year = date->year - yearShift;
  timeMark.tm_mon = date->month - monthShift;
  timeMark.tm_mday = date->day;
  timeMark.tm_hour = date->hour;
  timeMark.tm_min = date->minute;
  timeMark.tm_isdst = -1;

  return mktime(&timeMark);
}

int main() {
  const int ERR_CODE = -1;
  const int ERR_RETURN = 1;

  time_t previousTimestamp = 0;
  time_t currentTimestamp;
  time_t timeDelta;

  struct date date;
  memset(&date, 0, sizeof(date));

  /* Parse input lines */
  while (scanf("%d-%d-%d %d:%d", &date.year, &date.month, &date.day, &date.hour, &date.minute) != EOF) {
    currentTimestamp = getTimestamp(&date);
    if (currentTimestamp == ERR_CODE) return ERR_RETURN; 

    if (previousTimestamp) {
      /* Input time in minutes so we can safely divide by 60 */
      timeDelta = (currentTimestamp - previousTimestamp) / 60;
      printf("%ld\n", timeDelta);
    }

    previousTimestamp = currentTimestamp;
  }


  return 0;
}

