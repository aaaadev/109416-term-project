// SPDX-License-Identifier: MIT

#ifndef SINGLE_GAME_RESULT_H
#define SINGLE_GAME_RESULT_H

#include "../console/console.h"
#include "../console/page.h"
#include <stdlib.h>

struct ResultArgs {
  struct PageCtx *page_ctx;
  uint64_t *user_nums;
  uint64_t *winning_nums;
};

// PUBLIC fields

extern enum ConsoleResult on_view_single_game_result(struct PageCtx *page_ctx,
                                                     void *inner_args);

// PRIVATE fields

#endif