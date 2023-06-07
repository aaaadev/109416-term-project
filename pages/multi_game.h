// SPDX-License-Identifier: MIT

#ifndef MULTI_GAME_H
#define MULTI_GAME_H

#include "../console/console.h"
#include "../console/page.h"
#include <stdlib.h>

#include "../console/console.h"
#include "../console/page.h"
#include <stdlib.h>

enum FileErrorMulti {
  NOTHING_MULTI = 0b0,
  ALREADY_EXISTS_FILE = 0b10,
  NOT_VALID_FILE_NAME = 0b100,
  NOT_VALID_NUMBER_OF_GAMES = 0b1000,
  ERROR_FILE_OPEN = 0b10000,
};

static const char *MSG_MULTI = "< Multi game mode >\n";
static const char *MSG_MULTI2 =
    "Please enter a output filename and number of games.\nNumber of games should range between 1,000 to 1,000,000.\n";
static const char *MSG_ALREADY_EXISTS_MULTI =
    "File already exists. Try again.\n";
static const char *MSG_NOT_VALID_FILE_NAME_MULTI = "File name is not valid.\n";
static const char *MSG_NOT_VALID_NUMBER_OF_GAMES_MULTI =
    "Number of games is not valid.\n";
static const char *MSG_ERROR_FILE_OPEN_MULTI =
    "An error occurred while file open.\n";

struct HandleKeyDataMulti {
  struct PageCtx *page_ctx;
  size_t cur;
  char *filename;
  char *number_of_games;
};

// PUBLIC fields

extern enum ConsoleResult on_view_multi_game(struct PageCtx *page_ctx,
                                             void *args);

// PRIVATE fields

#endif