
/**
 * A tokenizer which parse the user inputs into tokens. TODO: the input
 * has to be explicitly 255 characters no cap.
 */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

#include "token.h"

/**
 * Get the size of the input
 */
int getInputSize(const char* input) {
  int i = 0; //counter
  while (input[i] != '\0') {
    i++;
  }
  return i;
}

/**
 * Is the given character a digit?
 */
int is_digit(char ch) {
  // this relies on the fact that digits are ordered in the ASCII table
  return ch >= '0' && ch <= '9';
}

/**
 * Read a word or an int from the provided input and put it in the output
 * array
 */
int addWord(const char *input, char *output) {
  int i = 0;

  // while there are inputs to read
  while (input[i] != '\0' && input[i] != ' ' && input[i] != '\n' && input[i] != ';') { 
    output[i] = input[i]; // copy character to output buffer
    ++i;
  }

  output[i] = '\0'; // add the terminating byte
  return i; // return the length of the string
}

/**
 * Get the size of the word being walked over
*/
int getWordSize(const char *input) {
  int i = 0;

  // while there are inputs to read
  while (input[i] != '\0' && input[i] != ' ' && input[i] != '\n' && input[i] != ';') {
    ++i;
  }
  return i; // return the length of the string
}

/**
 * Read a quote ended with a quotation mark or the end of the line
 */
int addQuote(const char *input, char *output) {
  int i = 0;

  // while there are inputs to read and it is not the end of the line
  // or the end of the quote
  while (input[i] != '\0' && input[i] != '\"') {
    output[i] = input[i]; // copy character to output buffer
    ++i;
  }
  output[i] = '\0'; // add the terminating byte

  return i; // return the length of the string
}

/**
 * Get the size of the quote thats being walked over
 */
int getQuoteSize(const char *input) {
  int i = 0;

  // while there are inputs to read and it is not the end of the line
  // or the end of the quote
  while (input[i] != '\0' && input[i] != '\"') {
    ++i;
  }

  return i; // return the length of the string
}

/**
 * Read input as a string and parse all tokens into an array. 
 */
char** tokenize(const char *input) {
  if (input == NULL) {
    printf("Cannot tokenize null\n");
    exit(1);
  }

  int inputSize = getInputSize(input);
  char** output = calloc(sizeof(char*) * getInputSize(input), sizeof(char));      // to store output values

  for (int k = 0; k < inputSize; k++) {      // pre-set NULL to all cells
    output[k] = NULL;
  }

  int i = 0;     // current index of the input reading pointer
  int j = 0;     // current index of the output reading pointer

  while (input[i] != '\0') {   // while it is not the end of the string
    if (input[i] != '\"' && (input[i] == '/' || isalpha(input[i])
       || input[i] == '-' || is_digit(input[i]))) { // if the current read is part of a word
      int wordSize = getWordSize(&input[i]);
      output[j] = malloc(sizeof(char*) * wordSize);
      i+=addWord(&input[i], output[j]);
      j++;
      continue;
    }

    // if not part of a chunk, check if it is part of special characters
    switch (input[i]) {
      case '(':
      case ')':
      case '>':
      case '<':
      case ';':
      case '|':
      case '\t':
        output[j] = calloc(1, sizeof(char*));
        output[j][0] = input[i];   // put the special character into the 
        j++;                     // output vector
        break;
      case '\"':
        i++;     // skip the quotation mark
        output[j] = malloc(sizeof(char*) * getQuoteSize(&input[i]));
        i += addQuote(&input[i], output[j]);     // read the quote into the output arr
        j++;
        break;
      case ' ':
      case '`':
      case '\'':
      case '\n':
        // skip spaces
        break;
      default:
        printf("ERROR: Unknown token '%c'\n", input[i]);
    }

    ++i; // advance to the next character
  }

  if (input[i] == EOF) {
    printf("detected eof 2\n");
    output[j] = "exit";
  }

  return output;
}

/**
 * Free all memory allocated to token (string arr)
 */
void freeToken(char **token) {
  assert(token != NULL);
  char** strings = token;

  // if the current string in the array is not null
  // free it and increment to the next string
  while (*strings != NULL) {
    free(*strings);
    ++strings;
  }
  free(token);
}
