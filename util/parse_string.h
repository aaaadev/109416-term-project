#ifndef PARSE_STRING_H
#define PARSE_STRING_H

#include <stdlib.h>

struct ParseResult {
  const char **list;
  size_t lines;
};

// PUBLIC fields

extern struct ParseResult parse_new_line(const char *_text,
                                         size_t max_per_line);

#endif