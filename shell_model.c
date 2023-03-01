
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "token.h"
#include "shell_helper.h"
#include "shell_model.h"

#define MAX_LINE_LENGTH 256

/**
 * A shell represents a model which contains basic info for running a mini-shell
*/
struct shell {
  vect_t* tokens;      // stores the current token
  char* prevInput;  // stores the previous token used
  int isOver;  // bool for whether or not the shell exe is over
                   // default active use of shell is non-zero int
                   // zero if the user exited shell
  int currToken;    // the index of the token currently processed
};

/**
 * Create and return a new shell
 */
shell_t* shell_new() {
  shell_t* shell = malloc(sizeof(shell_t));
  shell->tokens = vect_new();
  shell->prevInput = NULL;
  shell->isOver = 1;
  shell->currToken = 0;

  return shell;
}

/**
 * Free and delete the shell
 */
void shell_delete(shell_t* shell) {
  vect_delete(shell->tokens);
  free(shell);
}

/**
 * return the state of the shell (isOver)
 */
int shellState(shell_t* shell) {
  return shell->isOver;
}

/**
 * Add tokens to the shell in form of a string to a vector
 */
void addToken(shell_t* shell, char* input) {
  vect_add(shell->tokens, input);
}

/**
 * Get user input from the keyboard
 */
int tokenKeyboard(shell_t* shell) {
  char input[MAX_LINE_LENGTH];
  if (fgets(input, MAX_LINE_LENGTH, stdin) == NULL) {          // retrieve input from the keyboard
    shell->isOver = 0;
    return 0;
  }

  vect_add(shell->tokens, input);   // turn input into string and store in vector
}

/**
 * Get input from the previous token
 */
void tokenPrev(shell_t* shell) {
  assert(shell != NULL);
  assert(shell->prevInput != NULL);
  vect_add(shell->tokens, shell->prevInput);
}

/**
 * Find the end of an input in a sequence and return the index of the last token
 * before the end character
 */
int findEndInput(char** tokenized, int start) {
  if (start >= getTokenSize(tokenized)) {
    return 0;
  }

  int result = 0;
  result = start;  // the reported index

  // while the end of the sequence have not reached
  while (tokenized[result] != NULL && strcmp(tokenized[result], ";") != 0) {
    result++;
  }
  return result - 1;
}

/**
 * Return flag for special operators. 0 for regular, 1 for pipe and 2 for
 * redirection
 */
int getFlag(char** tokenized) {
  int i = 0;

  while (tokenized[i] != NULL) {
    // setting up flags for special operators
    if (strcmp(tokenized[i], "<") == 0) {
      return 3;
    } else if (strcmp(tokenized[i], ">") == 0) {
      return 2;
    } else if (strcmp(tokenized[i], "|") == 0) {
      return 1;
    }
    i++;
  }
  return 0;
}

/**
 * Parse tokens to the provided first and last indices
 * remember to free by the callee
 */
char** getParsedToken(char** tokenized, int first, int last) {
  assert(tokenized != NULL);
  assert(first <= last);
  int writeIdx = 0;

  char** result = calloc(last - first + 2, sizeof(char*)); // make space for each word
  for (int i = first; i <= last; i++) {
    result[writeIdx] = calloc(getLineSize(tokenized[i], 0) + 1, sizeof(char));
    strcpy(result[writeIdx], tokenized[i]);
    writeIdx++;
  }
  return result;
}

/**
 * Execute the command frammed by the provided indices and token
 */
int executeCommand(shell_t* shell, char** tokenized) {

  // check command in the first token index and act accordingly
  if (strcmp(tokenized[0], "exit") == 0) {             // exit the program
     shell->isOver = 0;
     freeToken(tokenized);
     if (shell->prevInput != NULL) {
       free(shell->prevInput);
     }
     return 0;

  } else if (strcmp(tokenized[0], "cd") == 0) {        // change directory
    if (chdir(tokenized[1]) != 0) {
    }

  } else if (strcmp(tokenized[0], "source") == 0) {      // get input from source file
    getSourceInput(tokenized[1], shell->tokens);
    freeToken(tokenized);
    return 0;

  } else if (strcmp(tokenized[0], "prev") == 0) {      // return the previous cmd
    tokenPrev(shell);
    freeToken(tokenized);
    return 0;

  } else if (strcmp(tokenized[0], "help") == 0) {      // print help manual
    if (runHelp() != 0) {
      printf("Error: Unable to print help manual\n");
    }

  } else {
    runSysCall(tokenized);
  }

  // passover and cleaning up
  if (shell->prevInput != NULL) {
    free(shell->prevInput);
  }
  shell->prevInput = makeString(tokenized);   // store the previous input
  freeToken(tokenized);
  return 0;
}

/**
 * Handles potential command sequencing
 */
int commSeq(shell_t* shell, char** tokenized) {
  int first = 0;   // index of the first token in the sequence
  int last = findEndInput(tokenized, 0);

  // while the current token's sequence have yet to be processed
  while (first <= last) {
    char** tempToken = getParsedToken(tokenized, first, last);
    int flag = getFlag(tempToken);

    switch (flag) {
      case 0:
        if (executeCommand(shell, tempToken) != 0) {
          printf("Error: executing regular command.\n");
          return 0;
        }
        break;
      case 1:
        if (pipeCommand(shell, tempToken) != 0) {
          printf("Error: executing pipe command.\n");
          return 0;
        }
        break;
      case 2:
        if (rdirCommand(shell, tempToken) != 0) {
          printf("Error: executing right redirection command. (1)\n");
          return 0;
        }
        break;
      case 3:
        if (ldirCommand(shell, tempToken) != 0) {
          printf("Error: executing left redirection command.\n");
          return 0;
        }
        break;
      default:
        printf("Error: unrecognized operator\n");
        return 0;
    }

    first = last + 2;
    last = findEndInput(tokenized, first);
  }
  freeToken(tokenized);
  return 0;
}

/**
 * Return the index of the last token before |, <, > or the end of the array
 * for first command
 */
int getSpecialInput(char** tokenized, int start) {
  int i = start;

  while (tokenized[i] != NULL && strcmp(tokenized[i], "|") != 0
  && strcmp(tokenized[i], ">") != 0 && strcmp(tokenized[i], "<") != 0) {
    i++;
  }
  i -= 1;  // remove the non-wanted "|" at the end
  return i;
}

/**
 * Return the index of the last token before the pipe or end of the array
 * for second command
 */
int getEndInput(char** tokenized, int start) {
  int i = start;

  while (tokenized[i] != NULL) {
    i++;
  }
  i -= 1;  // remove the non-wanted "|" at the end
  return i;
}

/**
 * Handles the left pointed redirection of commands
 */
int ldirCommand(shell_t* shell, char** tokenized) {
  int startCom1 = 0;
  int endCom1 = getSpecialInput(tokenized, startCom1);
  int startCom2 = endCom1 + 2;
  int endCom2 = getEndInput(tokenized, startCom2);

  pid_t child = fork();
  if (child == 0) {  // inside the child
    fflush(stdin);  // close stdin so we can take input from file
    fflush(stdout);
    close(0);

    // retrieve the second token (besides <) in the command
    char** tokens2 = getParsedToken(tokenized, startCom2, endCom2);
    int fd = open(tokens2[0], O_RDONLY);   // open file to take input
    assert(fd == 0);

    char** tokens1 = getParsedToken(tokenized, startCom1, endCom1);
    if (commSeq(shell, tokens1) != 0) {  // execute the comms recursively
      printf("Error: execute left redirection failed\n");
      return 1;
    }
    freeToken(tokens2);
    exit(0);
  }

  // in parent
  wait(NULL);
  freeToken(tokenized);
  return 0;
}

/**
 * Handles the right pointed redirection of commands
 */
int rdirCommand(shell_t* shell, char** tokenized) {
  int startCom1 = 0;
  int endCom1 = getSpecialInput(tokenized, startCom1);
  int startCom2 = endCom1 + 2;
  int endCom2 = getEndInput(tokenized, startCom2);

  pid_t child = fork();
  if (child == 0) {  // inside the child
    fflush(stdout);   // clear values in stdout
    fflush(stdin);
    close(1);

    // open respective file for writing, creating, truncating
    // in place of stdout
    char** tokens2 = getParsedToken(tokenized, startCom2, endCom2);
    int fd = open(tokens2[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    assert(fd == 1);

    char** tokens1 = getParsedToken(tokenized, startCom1, endCom1);
    if (commSeq(shell, tokens1) != 0) {
      printf("Error: execute right redirection failed (2)\n");
      return 1;
    }
    freeToken(tokens2);
    exit(0);
  }
  // in parent
  wait(NULL);
  freeToken(tokenized);
  return 0;
}

/**
 * Handles the piping of commands
 */
int pipeCommand(shell_t* shell, char** tokenized) {
    int startCom1 = 0;
    int endCom1 = getSpecialInput(tokenized, startCom1);
    int startCom2 = endCom1 + 2;
    int endCom2 = getEndInput(tokenized, startCom2);

    int child_pid = fork();

    if (child_pid > 0) { // in parent
      wait(NULL);
      freeToken(tokenized);
      return 0;
    }
    else if (child_pid == 0) { // in child A
      int pipe_fds[2]; // the pipe system call creates two file descriptors in the 2-element
                     // array given as argument

      assert(pipe(pipe_fds) == 0); // returns 0 on success

      int read_fd = pipe_fds[0]; // index 0 is the "read end" of the pipe
      int write_fd = pipe_fds[1]; // index 1 is the "write end" of the pipt

      if (fork() == 0) {        // forking child B
        fflush(stdout);
        close(read_fd); // close the other end of the pipe

        // replace stdout with the write end of the pipe
        if (close(1) == -1) {
          perror("Error closing stdout");
          exit(1);
        }
        assert(dup(write_fd) == 1);
        // execute command 1
        char** tokens1 = getParsedToken(tokenized, startCom1, endCom1);

        commSeq(shell, tokens1);

        close(1);
        close(pipe_fds[1]);
        exit(0);
      }

      // child A content:
      fflush(stdin);
      close(write_fd); // close the other end of the pipe

      // replace stdin with the read end of the pipe
      if (close(0) == -1) {
        perror("Error closing stdin");
        exit(1);
      }
      assert(dup(read_fd) == 0);

      // execute command 2
      char** tokens2 = getParsedToken(tokenized, startCom2, endCom2);

      commSeq(shell, tokens2);
      wait(NULL);

      close(0);
      close(pipe_fds[0]);
      exit(0);
    }
    else {
      perror("Error - fork failed");
      exit(1);
    }

    return 0;
}

/**
 * controller which handles the input provided for shell. assumes shell
 * is already loaded with tokens (number of tokens >= 1);
 */
int shellController(shell_t* shell) {
  assert(shell != NULL);
  // while there are tokens to be processed and the shell is not over
  while (shell->currToken < vect_size(shell->tokens) && shell->isOver != 0) { // while there are tokens yet to process
    char* currToken = NULL;
    currToken = vect_get_copy(shell->tokens, shell->currToken);
    assert(currToken != NULL);

    char** tokenized =  tokenize(currToken);  // get the token from the current string
    free(currToken);

    if (commSeq(shell, tokenized) != 0) {
      printf("Error: unable to run command\n");
    }
    shell->currToken++;   // go to the next token in the vector
  }

  // clear up old shell values for next run
  vect_delete(shell->tokens);
  shell->tokens = vect_new();
  shell->currToken = 0;

  return 0;
}
