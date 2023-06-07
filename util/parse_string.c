// SPDX-License-Identifier: MIT

#include "parse_string.h"
#include <stdlib.h>
#include <string.h>

struct ParseResult parse_new_line(const char *_text, size_t max_per_line) {
  char *text = strdup(_text);
  char **list = malloc(sizeof(char *));
  size_t lines = 0;
  size_t char_cnt = 0;
  while (*text != '\0') {
    char_cnt++;
    if (*text == '\n') {
      *text = '\0';
      list = realloc(list, sizeof(char *) * (lines + 1));
      list[lines] = malloc(sizeof(char) * (char_cnt + 1));
      memcpy(list[lines], text - (char_cnt - 1), char_cnt + 1);
      char_cnt = 0;
      lines++;
    } else if (char_cnt == max_per_line) {
      list = realloc(list, sizeof(char *) * (lines + 1));
      list[lines] = malloc(sizeof(char) * (char_cnt + 1));
      memcpy(list[lines], text - (char_cnt - 1), char_cnt);
      list[lines][char_cnt] = '\0';
      char_cnt = 0;
      lines++;
    }
    text++;
  }
  if (char_cnt > 0) {
    list = realloc(list, sizeof(char *) * (lines + 1));
    list[lines] = malloc(sizeof(char) * (char_cnt + 1));
    memcpy(list[lines], text - (char_cnt), char_cnt);
    char_cnt = 0;
    lines++;
  }
  return (struct ParseResult){list, lines};
}