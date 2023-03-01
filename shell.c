#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#include "token.h"
#include "shell_helper.h"
#include "shell_model.h"

#define MAX_LINE_LENGTH 256

/**
 * Implements mini shell.
 */
int main(int argc, char **argv) {
  shell_t* shell = shell_new();

  // welcome message
  printf("Welcome to mini-shell.\n");

  while (shellState(shell) != 0) { // while the shell is not over
    printf("shell $ ");
    tokenKeyboard(shell);  // get tokens from the keyboard

    if (shellController(shell) != 0) {
      printf("Error: trouble executing tokens\n");
    }
  }

  // exit message
  printf("Bye bye.\n");
  shell_delete(shell);
  return 0;
}
