#ifndef MULTI_GAME_RESULT_H
#define MULTI_GAME_RESULT_H

#include "../console/console.h"
#include "../console/page.h"
#include <stdint.h>
#include <stdlib.h>

struct ResultArgsMulti {
  struct PageCtx *page_ctx;
  uint64_t total_balance;
  size_t first_prize;
  size_t second_prize;
  size_t third_prize;
  size_t forth_prize;
  size_t no_prize;
};

// PUBLIC fields

extern enum ConsoleResult on_view_multi_game_result(struct PageCtx *page_ctx,
                                                    void *inner_args);

// PRIVATE fields

#endif