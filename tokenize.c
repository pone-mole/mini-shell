/**
 * A tokenizer which parse the user inputs into tokens. The input
 * has to be explicitly 255 characters no cap.
 */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

#include "token.h"

// run the tokenizer to parse tokens from the keyboard input
int main(int argc, char **argv) {
  char input[256];
  fgets(input, 256, stdin);          // retrieve input from the keyboard

  char** tokens = tokenize(input);   // parse the input

  assert(tokens != NULL);

  char** print = tokens;
  // print all elements of the output out to the terminal
  while (*print != NULL) {
    printf("%s\n", *print);
    ++print;
  }

  freeToken(tokens);
  return 0;
}
