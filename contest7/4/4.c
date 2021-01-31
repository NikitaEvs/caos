#include <dirent.h>    
#include <errno.h>     
#include <grp.h>
#include <math.h>
#include <stdlib.h>     
#include <stdio.h>        
#include <sys/types.h>    
#include <sys/stat.h>
#include <string.h>       
#include <stdint.h> 
#include <fcntl.h>
#include <limits.h>    
#include <locale.h>
#include <unistd.h>
#include <pwd.h>

/**
 * Representation of ls -l line for the file
 */
struct fileDescription {
  char* attributeStr;
  unsigned long int linkNumber;
  char* usernameStr;
  char* groupnameStr;
  unsigned long int size;
  char* filenameStr;

  int isLink;
};


/**
 * Filter all directories and ./ files
 * @param entry
 * @return 1 if file is valed, 0 otherwise
 */
static int filter(const struct dirent *entry) {
    /* Skip entries that begin with '.' */
    if (entry->d_name[0] == '.')
        return 0;

    /* Include all others */
    return 1;
}


/**
 * Fill fileDescription structure using stat information
 * @param fileInfo
 * @param fileDescription
 * @return on success 0, otherwise -1
 */
int getInfo(const struct stat *fileInfo, struct fileDescription *description) {
  const int ERR_CODE = -1;

  struct passwd *owner;
  struct group *group;

  /* Get file mode human-readable info */
  const int ATTRIBUTE_STR_SIZE = 11;
  char *attributeStr = malloc(ATTRIBUTE_STR_SIZE * sizeof(char));
  memset(attributeStr, 0, ATTRIBUTE_STR_SIZE);

  description->isLink = 0;
  if (S_ISDIR(fileInfo->st_mode)) {
    sprintf(attributeStr, "d"); 
  } else if (S_ISLNK(fileInfo->st_mode)) {
    description->isLink = 1;
    sprintf(attributeStr, "l");
  } else if (S_ISCHR(fileInfo->st_mode)) {
    sprintf(attributeStr, "c");
  } else if (S_ISBLK(fileInfo->st_mode)) {
    sprintf(attributeStr, "b");
  } else if (S_ISFIFO(fileInfo->st_mode)) {
    sprintf(attributeStr, "p");
  } else if (S_ISSOCK(fileInfo->st_mode)) {
    sprintf(attributeStr, "s");
  } else {
    sprintf(attributeStr, "-");
  }

  /* I know, it's a magic number, but it's really hard to write it in a pretty way */
  sprintf(attributeStr + 1, (fileInfo->st_mode & S_IRUSR) ? "r" : "-");
  sprintf(attributeStr + 2, (fileInfo->st_mode & S_IWUSR) ? "w" : "-");
  sprintf(attributeStr + 3, (fileInfo->st_mode & S_IXUSR) ? "x" : "-");
  sprintf(attributeStr + 4, (fileInfo->st_mode & S_IRGRP) ? "r" : "-");
  sprintf(attributeStr + 5, (fileInfo->st_mode & S_IWGRP) ? "w" : "-");
  sprintf(attributeStr + 6, (fileInfo->st_mode & S_IXGRP) ? "x" : "-");
  sprintf(attributeStr + 7, (fileInfo->st_mode & S_IROTH) ? "r" : "-");
  sprintf(attributeStr + 8, (fileInfo->st_mode & S_IWOTH) ? "w" : "-");
  sprintf(attributeStr + 9, (fileInfo->st_mode & S_IXOTH) ? "x" : "-");

  description->attributeStr = attributeStr;

  /* Get a number of hard links */
  description->linkNumber = fileInfo->st_nlink;
  
  /* Get an owner */
  errno = 0; // Need to set to zero according to the man 
  owner = getpwuid(fileInfo->st_uid);
  if (!owner) {
    return ERR_CODE;
  }
  description->usernameStr = malloc(PATH_MAX * sizeof(char));
  strcpy(description->usernameStr, owner->pw_name);

  /* Get a group */
  errno = 0; // Need to set to zero according to the man 
  group = getgrgid(fileInfo->st_gid);
  if (!group) {
    return ERR_CODE;
  }
  description->groupnameStr = malloc(PATH_MAX * sizeof(char));
  strcpy(description->groupnameStr, group->gr_name);

  /* Get a file size */
  description->size = fileInfo->st_size;

  return 0;
}

/**
 * Print fileDescription lines about files in the directory
 * @param descriptions
 * @param entriesNumber : size of descriptions array
 */
void prettyPrint(struct fileDescription **descriptions, int entriesNumber) {
  int maxLinkNumberLength = 1;
  int maxSizeLength = 1;
  int maxNameLength = 1;
  int maxGroupLength = 1;

  /* Calculate width of columns for linkNumber and size */
  for (int pos = 0; pos < entriesNumber; ++pos) {
    if(!descriptions[pos]) continue;

    unsigned long int linkNumber  = descriptions[pos]->linkNumber;
    unsigned long int size        = descriptions[pos]->size;
    
    int linkNumberLength = 1;
    int sizeLength = 1;
    int nameLength = 1;
    int groupLength = 1;

    if (linkNumber != 0) {
      linkNumberLength = floor(log10(linkNumber)) + 1;
    }

    if (size != 0) {
      sizeLength = floor(log10(size)) + 1;
    }

    nameLength  = strlen(descriptions[pos]->usernameStr);
    groupLength = strlen(descriptions[pos]->groupnameStr);

    if (linkNumberLength > maxLinkNumberLength) {
      maxLinkNumberLength = linkNumberLength;
    }

    if (sizeLength > maxSizeLength) {
      maxSizeLength = sizeLength;
    }

    if (nameLength > maxNameLength) {
      maxNameLength = nameLength;
    }

    if (groupLength > maxGroupLength) {
      maxGroupLength = groupLength;
    }
  }

  /* Print lines of descriptions */
  for (int pos = 0; pos < entriesNumber; ++pos) {
    if(!descriptions[pos]) continue;

    struct fileDescription *description = descriptions[pos];

    printf("%s", description->attributeStr);
    printf(" ");
    printf("%*lu", maxLinkNumberLength, description->linkNumber);
    printf(" ");
    printf("%-*s", maxNameLength, description->usernameStr);
    printf(" ");
    printf("%-*s", maxGroupLength, description->groupnameStr);
    printf(" ");
    printf("%*lu", maxSizeLength, description->size);
    printf(" ");
    printf("%s", description->filenameStr);
    printf("\n");
  }
}

/**
 * Print to stdout description of all entries of the directory in path
 * @param path
 * @return on success 0, otherwise -1
 */
int ls(char *path) {
  const int ERR_CODE = -1;

  int directoryFd;
  DIR *directory;
  struct dirent **entries;
  struct dirent *entry;
  int entriesNumber;

  struct stat fileInfo;
  struct fileDescription **descriptions;

  /* Check type of the file */
  if (lstat(path, &fileInfo) == ERR_CODE) {
    goto error;
  }

  /* Proccess with a non-directory file */
  if (!S_ISDIR(fileInfo.st_mode)) {
    descriptions = malloc(sizeof(struct fileDescription*));
    descriptions[0] = malloc(sizeof(struct fileDescription));
    if (getInfo(&fileInfo, *descriptions) == ERR_CODE) {
      goto error;
    }

    descriptions[0]->filenameStr = path;

    prettyPrint(descriptions, 1);

    free(descriptions[0]->attributeStr);
    free(descriptions[0]);
    free(descriptions);

    return 0;
  }


  /* Open the directory */         
  directoryFd = open(path, O_RDONLY | O_DIRECTORY);
  directory = fdopendir(directoryFd);
  if (!directory) {
    goto error;
  }                                              
                                              
  /* Read entries from the directory and add its name to the */     
  entriesNumber = scandir(path, &entries, filter, alphasort);
  if (entriesNumber == ERR_CODE) {
    goto error;
  }
  descriptions = malloc(entriesNumber * sizeof(struct fileDescription*));

  /* Iterate over the entries array and print information about current entry */
  for (int pos = 0; pos < entriesNumber; ++pos) {
    entry = entries[pos];

    descriptions[pos] = malloc(sizeof(struct fileDescription));

    if (fstatat(directoryFd, entry->d_name, &fileInfo, AT_SYMLINK_NOFOLLOW) == ERR_CODE) {
      goto freeError;
    }

    memset(descriptions[pos], 0, sizeof(struct fileDescription));

    /* Get information about the file */
    if (getInfo(&fileInfo, descriptions[pos]) == ERR_CODE) {
      goto freeError;
    }

    /* Get a filename string */
    if (descriptions[pos]->isLink) {
      char *symlinkFileName = malloc(PATH_MAX * sizeof(char));
      char *entrySymlinkFilename = malloc(PATH_MAX * sizeof(char));
      char *newFileName = malloc(PATH_MAX * sizeof(char));
      memset(symlinkFileName, 0, PATH_MAX);
      memset(entrySymlinkFilename, 0, PATH_MAX);
      memset(newFileName, 0, PATH_MAX);

      strcat(entrySymlinkFilename, path);
      strcat(entrySymlinkFilename, "/");
      strcat(entrySymlinkFilename, entry->d_name);

      if (!realpath(entrySymlinkFilename, symlinkFileName)) {
        goto freeError;
      }

      strcat(newFileName, entry->d_name);
      strcat(newFileName, " -> ");
      strcat(newFileName, symlinkFileName);

      descriptions[pos]->filenameStr = newFileName;
      free(symlinkFileName);
      free(entrySymlinkFilename);
    } else {
      char *buffer = malloc(PATH_MAX * sizeof(char));
      memset(buffer, 0, PATH_MAX);
      strcat(buffer, entry->d_name);
      
      descriptions[pos]->filenameStr = buffer;
    }

    free(entry);
  }
  free(entries); 

  prettyPrint(descriptions, entriesNumber);

  if (closedir(directory) == ERR_CODE) {    
    goto error;
  }

  /* Free memory */
  for (int pos = 0; pos < entriesNumber; ++pos) {
    if (descriptions[pos]) {
      free(descriptions[pos]->attributeStr);
      free(descriptions[pos]->filenameStr);
      free(descriptions[pos]->usernameStr);
      free(descriptions[pos]->groupnameStr);
      free(descriptions[pos]);
    }
  }
  free(descriptions);

  return 0;

freeError:
  free(entry);
  free(entries); 

  if (descriptions) {
    for (int pos = 0; pos < entriesNumber; ++pos) {
      if (descriptions[pos]) {
        free(descriptions[pos]->attributeStr);
        free(descriptions[pos]->filenameStr);
        free(descriptions[pos]->usernameStr);
        free(descriptions[pos]->groupnameStr);
        free(descriptions[pos]);
      }
    }
    free(descriptions);
  }

error:
  perror(NULL);    
  return ERR_CODE;    
}


int main(int argc, char** argv) {    
  /* This is necessary for alphasort because without it 
   * alphasort will sort considering capital letter */
  setlocale(LC_ALL, "");

  const int ERR_CODE = -1;    
  const int ERR_RETURN = 1;    
      
  const int NUMBER_OF_ARGUMENTS = 2;    
  const int DIR_POSITION = 1;    
     
  char* dirName;        
     
  if (argc < NUMBER_OF_ARGUMENTS) {    
    return ERR_RETURN;    
  }    
  dirName = argv[DIR_POSITION];    
  
  if(ls(dirName) == ERR_CODE) {
    return ERR_RETURN;
  }

  return 0;    
}    

