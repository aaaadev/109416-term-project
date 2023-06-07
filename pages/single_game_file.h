// SPDX-License-Identifier: MIT

#ifndef SINGLE_GAME_FILE_H
#define SINGLE_GAME_FILE_H

#include "../console/console.h"
#include "../console/page.h"
#include <stdlib.h>

enum FileError {
  NOTHING,
  INVALID_FILE_NAME,
  INVALID_FILE_FORMAT,
};

static const char *MSG_FILE =
    "< Single game mode >\nPlease enter a valid filename has 6 integers "
    "between 1 to 45 which is separated by whitespace\n";
static const char *MSG_INVALID_FILE_NAME_FILE =
    "< Single game mode >\nInvalid file name. Try again.\nPlease enter a valid "
    "filename has 6 integers between 1 to 45 which is separated by "
    "whitespace\n";
static const char *MSG_INVALID_FILE_FORMAT_FILE =
    "< Single game mode >\nInvalid file format. Try again.\nPlease enter a "
    "valid filename has 6 integers between 1 to 45 which is separated by "
    "whitespace\n";

struct HandleKeyDataFile {
  struct PageCtx *page_ctx;
  size_t cur;
  char *str;
};

// PUBLIC fields

extern enum ConsoleResult on_view_single_game_file(struct PageCtx *page_ctx,
                                                   void *args);

// PRIVATE fields

#endif