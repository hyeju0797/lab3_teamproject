#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd.h"

//initialize
cmdgroup_t *create_cmdgroup() {
  cmdgroup_t *group = malloc(sizeof(cmdgroup_t));
  group->tokens = NULL;
  group->delimeter = ' ';
  group->stdin_redirect_file = NULL;
  group->stdout_redirect_file = NULL;
  group->pipe_fd[0] = -1;
  group->pipe_fd[1] = -1;

  return group;
}

//initialize
strtokens_t *create_strtokens() {
  strtokens_t *tokens = malloc(sizeof(strtokens_t));
  tokens->length = 0;

  for (int i = 0; i < MAX_TOKEN_LENGTH; i++) {
    tokens->elements[i] = NULL;
  }

  return tokens;
}


strtokens_t *slice(strtokens_t *tokens, int start_idx, int end_idx) {
  strtokens_t *sliced = create_strtokens();

  for (int i = start_idx; i < end_idx; i++) {
    add_element(sliced, tokens->elements[i]);
  }

  return sliced;
}

void split(strtokens_t *tokens, char *input_str, const char *delimiters) {
  int i = 0;
  int numtokens = 0;
  char *snew = NULL;

  if((input_str==NULL) || (delimiters==NULL)) return;

  snew = input_str + strspn(input_str, delimiters);
  char *element = strtok(snew, delimiters);
  if(element == NULL) {
    return;
  }
  do {
    add_element(tokens, element);
    element = strtok(NULL, delimiters);
  } while ((element != NULL) && (tokens->length < MAX_TOKEN_LENGTH));
}

void add_element(strtokens_t *tokens, char *new_element) {
  tokens->elements[tokens->length++] = new_element;
}
