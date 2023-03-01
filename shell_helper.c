
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#include "token.h"
#include "vect.h"
#include "shell_helper.h"

/**
 * Return the content of the file as a String
 */
char* openFile(const char* fileName) {
  FILE* file = fopen(fileName, "r");
  if (file == NULL) {   // check if the file was properly read
        printf("Error reading file %s\n", fileName);
        exit(1);
  }

  fseek(file, 0L, SEEK_END);    // move to the end of the file
  long int size = ftell(file);  // to check the size of the file

  fseek(file, -size, SEEK_END); // move the read pointer to the front
  char content = fgetc(file);
  char* output = malloc(size + 1);

  int i = 0;                  // convert char to char*
  while (content != EOF) {
    output[i] = content;
    content = fgetc(file);
    i++;
  }
  output[i] = '\0';

  fclose(file);
  return output;
}

/**
 * Get the size/length of string up to new line or the end of the string.
*/
int getLineSize(char* input, int readIdx) {
  int i = readIdx;      // current reading index

  while (input[i] != '\n' && input[i] != '\0') {
    i++;
  }

  return i - readIdx;
}

/**
 * Parse inputs in a source file by line and add to source vector.
 */
void getSourceInput(char* fileName, vect_t* sourceVect) {
  char* content = openFile(fileName);
  char* currLine = NULL;
  int readIdx = 0;
  int writeIdx = 0;

  // while there is something to read
  while (content[readIdx] != '\0') {
    currLine = calloc(getLineSize(content, readIdx) + 1, sizeof(char));

    while (content[readIdx] != '\n' && content[readIdx] != '\0') {  // record line
      currLine[writeIdx] = content[readIdx];
      writeIdx++;
      readIdx++;
    }

    // add the line to vector
    currLine[writeIdx] = '\0';
    vect_add(sourceVect, currLine);
    writeIdx = 0;
    readIdx++;
    free(currLine);     // reset the memo line
  }
  free(content);
}

/**
 * Print the help manual to the user.
 */
int runHelp() {
  // Read contents from file
  char* content = openFile("help.txt");
  if (content == NULL) {
    return 1;
  }

  int i = 0;
  while (content[i] != '\0') {       // read until the end of file
    printf ("%c", content[i]);
    i++;
  }

  free(content);
  return 0;
}

/**
 * Run system call with provided input. Print error if the call does not
 * go through.
 */
int runSysCall(char** input) {
  if (fork() == 0) {
    assert(execvp(input[0], input) != 1);
    printf("%s: command not found\n", input[0]);  // should not reach here
    return 1;
  } else {
    wait(NULL);
  }

  return 0;
}

/**
 * Return the provided String arr as a String
 */
char* makeString(char** strArr) {
  int i = 0;
  int j = 0;
  int k = 0;
  int strSize = 0;

  // get the size of all strings in the strArr
  while (strArr[i] != NULL) {
    while (strArr[i][j] != '\0') {
      strSize++;
      j++;
    }
    i++;
    j = 0;
    strSize++;
  }
  i = 0;    // resets the reading idx
  j = 0;

  char* result = calloc(strSize + 1, sizeof(char));

  while (strArr[i] != NULL) {
    while (strArr[i][j] != '\0') {
      result[k] = strArr[i][j];
      k++;
      j++;
    }

    if (k < strSize) {    // there is another word down
      result[k] = ' ';
    }
    k++;
    i++;
    j = 0;
  }
  return result;
}

/**
 * Get the size of the token
 */

int getTokenSize(char** tokenized) {
  int i = 0;

  while (tokenized[i] != NULL) {
    i++;
  }
  return i;
}
