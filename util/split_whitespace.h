// SPDX-License-Identifier: MIT

#ifndef SPLIT_WHITESPACE_H
#define SPLIT_WHITESPACE_H

#include <stdlib.h>
#include <string.h>

struct SplitResult {
  char **data;
  size_t length;
};

// PUBLIC fields

extern struct SplitResult split_whitespace(const char *text);

#endif