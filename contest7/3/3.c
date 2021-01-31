/**
 * Move suffix starting in the suffix pointer to the base pointer
 * @param base
 * @param suffix
 */
void shift(char *base, char *suffix) {
  do {
    *base = *suffix;
    *suffix = 0;
    ++base;
    ++suffix;
  } while (*suffix);
  *base = 0;
}

/**
 * Move pointer base to the next delimiter or to the terminal of the string
 * @param base
 * @param delimiter
 * @return moved pointer
 */
char *forwardToDelimiter(char *base, char delimiter) {
  while (*base != delimiter) {
    ++base;

    if (!(*base)) break;
  }

  return base;
}

/**
 * Move backward pointer base to the next delimiter or to the start pointer 
 * @param base
 * @param start
 * @param delimiter
 * @return moved pointer
 */
char *backwardToDelimiter(char *base, const char *start, char delimiter) {
  while (*base != delimiter) {
    if(base == start) {
      return base;
    }

    --base;
  }

  return base + 1;
}

/**
 * Check that the pattern is a prefix of the text 
 * @param pattern 
 * @param text
 * @return 1 if the pattern is a prefix of the text, 0 otherwise
 */
int isPrefix(const char *pattern, const char *text) {
  while (*pattern) {
    if (*pattern != *text) return 0;

    ++pattern;
    ++text;
  }

  return 1; 
}

/**
 * Normalize a given path
 * Resolve ../ and ./
 * Delete multiply / in a row 
 * @param path
 * Note: this function modifies given path
 */
extern void normalize_path(char *path) {
  char *ptr = path;

  const char delimiter = '/';
  const char *currentDir = "./";
  int currentDirShift = 2;

  const char *previousDir = "../";
  int previousDirShift = 3;

  while (*ptr) {
    if (*ptr == delimiter) {
      char *shifted = ptr;
      while (*(shifted + 1) == delimiter) {
        ++shifted;
      }

      if (shifted != ptr) {
        shift(ptr, shifted);
      }

      ++ptr;
    } else if (isPrefix(currentDir, ptr)) {
      shift(ptr, ptr + currentDirShift);
      ++ptr;
    } else if (isPrefix(previousDir, ptr)) {
      char *previous = backwardToDelimiter(ptr - 2, path, delimiter);
      shift(previous, ptr);
      shift(previous, previous + previousDirShift); 

      ptr = previous;
    } else {
      ++ptr;
    }
  }
}

//#include <stdio.h>
//
//int main(int argc, char** argv) {
//  char *path = argv[1];
//  normalize_path(path);
//
//  printf("%s\n", path); 
//
//  return 0;
//}

