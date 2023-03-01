#ifndef _TOKEN_H
#define _TOKEN_H

/**
 * A tokenizer which parse the user inputs into tokens. TODO: the input
 * has to be explicitly 255 characters no cap.
 */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

/**
 * Get the size of the input
 */
int getInputSize(const char* input);

/**
 * Is the given character a digit?
 */
int is_digit(char ch);

/**
 * Read a word or an int from the provided input and put it in the output
 * array
 */
int addWord(const char *input, char *output);

/**
 * Get the size of the word being walked over
*/
int getWordSize(const char *input);

/**
 * Read a quote ended with a quotation mark or the end of the line
 */
int addQuote(const char *input, char *output);

/**
 * Get the size of the quote thats being walked over
 */
int getQuoteSize(const char *input);

/**
 * Read input as a string and parse all tokens into an array. 
 */
char** tokenize(const char *input);

/**
 * Free all memory allocated to token (string arr)
 */
void freeToken(char **token);

#endif
