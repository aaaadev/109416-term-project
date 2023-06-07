#include "split_whitespace.h"
#include <stdlib.h>
#include <string.h>

struct SplitResult split_whitespace(const char *text) {
  char *str = strdup(text);
  char *prev = str;
  char **data = malloc(sizeof(char *));
  size_t length = 0;
  char *token = strtok(str, " ");
  while (token != NULL) {
    data = realloc(data, sizeof(char *) * (length + 1));
    data[length] = strdup(token);
    length++;
    token = strtok(NULL, " ");
  }
  return (struct SplitResult){data, length};
}