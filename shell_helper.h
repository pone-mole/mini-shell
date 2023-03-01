#ifndef _SHELL_HELPER_H
#define _SHELL_HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#include "token.h"
#include "vect.h"

/**
 * Return the content of the file as a String
 */
char* openFile(const char* fileName);

/**
 * Get the size/length of string up to new line or the end of the string.
*/
int getLineSize(char* input, int readIdx);

/**
 * Parse inputs in a source file by line and add to source vector.
 */
void getSourceInput(char* fileName, vect_t* sourceVect);

/**
 * Print the help manual to the user.
 */
int runHelp();

/**
 * Run system call with provided input. Print error if the call does not
 * go through.
 */
int runSysCall(char** input);

/**
 * Return the provided String arr as a String
 */
char* makeString(char** strArr);

/**
 * Get the size of the token
 */

int getTokenSize(char** tokenized);

#endif
