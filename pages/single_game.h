// SPDX-License-Identifier: MIT

#ifndef SINGLE_GAME_H
#define SINGLE_GAME_H

#include "../console/console.h"
#include "../console/page.h"
#include <stdlib.h>

static const char *MSG =
    "< Single game mode >\nPlease enter 6 integers between 1 to 45 which is "
    "separated by whitespace.\n";
static const char *MSG_INVALID =
    "< Single game mode >\nInvalid input. Try again.\nPlease enter 6 "
    "integers "
    "between 1 to 45 which is separated by whitespace.\n";

struct HandleKeyData {
  struct PageCtx *page_ctx;
  size_t cur;
  char *str;
};

// PUBLIC fields

extern enum ConsoleResult on_view_single_game(struct PageCtx *page_ctx,
                                              void *args);

// PRIVATE fields

#endif