#ifndef _SHELL_MODEL_H
#define _SHELL_MODEL_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "token.h"
#include "shell_helper.h"

#define MAX_LINE_LENGTH 256

// define a shell which is a model with its fields hidden
typedef struct shell shell_t;

/**
 * Create and return a new shell
 */
shell_t* shell_new();

/**
 * Return flag for special operators. 0 for regular, 1 for pipe and 2 for
 * redirection
 */
int getFlag(char** tokenized);

/**
 * Return the index of the last token before the pipe or end of the array
 * for first command
 */
int getPipeInput1(char** tokenized, int start);

/**
 * Return the index of the last token before the pipe or end of the array
 * for second command
 */
int getPipeInput2(char** tokenized, int start);

/**
 * Free and delete the shell
 */
void shell_delete(shell_t* shell);

/**
 * Execute the command involving pipe
 */
int pipeCommand(shell_t* shell, char** tokenized);

/**
 * Handle the right pointed (>) redirection command
 */
int rdirCommand(shell_t* shell, char** tokenized);

/**
 * Handle the left pointed (<) redirection command
 */
int ldirCommand(shell_t* shell, char** tokenized);

/**
 * Get user input from the keyboard
 */
int tokenKeyboard(shell_t* shell);

/**
 * Exit the shell
 */
void exitShell(shell_t* shell);

/**
 * Get input from the previous token
 */
void tokenPrev(shell_t* shell);

/**
 * controller which handles the input provided for shell. assumes shell
 * is already loaded with tokens (number of tokens >= 1);
 */
int shellController(shell_t* shell);

#endif
