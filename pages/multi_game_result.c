#include "multi_game_result.h"
#include "../console/console.h"
#include "../console/menu.h"
#include "../console/page.h"
#include "../debug.h"
#include "../util/prize.h"
#include "main_menu.h"
#include <stdio.h>
#include <stdlib.h>

// PRIVATE

enum ConsoleResult on_ok_multi_game_result(const struct MenuCtx *menu) {
  enum ConsoleResult res = CRESULT_SUCCESS;
  struct PageCtx *page_ctx = menu->page_ctx;
  if (menu->current_select == 1) {
    size_t idx = add_page(page_ctx, on_view_main, true);
    update_result(&res, navigate_page(page_ctx, idx, NULL, 0));
  } else if (menu->current_select == 0) {
    update_result(&res, popback_page(page_ctx));
  } else {
#ifdef DEBUG
    DPRINTF("unknown item select handle\n");
    fflush(stdout);
#endif
  }
  return res;
}

enum ConsoleResult on_view_multi_game_result(struct PageCtx *page_ctx,
                                             void *inner_args) {
  enum ConsoleResult res = CRESULT_SUCCESS;
  struct ResultArgsMulti *args = inner_args;
  char body[4096];
  sprintf(body,
          "first prize: %lld\nsecond prize: %lld\nthird prize: %lld\nforth "
          "prize: %lld\nno prize: %lld\ntotal balance: %lld won",
          args->first_prize, args->second_prize, args->third_prize,
          args->forth_prize, args->no_prize, args->total_balance);
  struct MenuCtx *menu = menu_init(page_ctx->ctx, page_ctx, "Game result", body,
                                   on_ok_multi_game_result);
  struct MenuItem retry_item = (struct MenuItem){
      "retry",
      false,
  };
  struct MenuItem ok_item = (struct MenuItem){
      "ok",
      false,
  };
  update_result(&res, add_item(menu, (const struct MenuItem)retry_item));
  update_result(&res, add_item(menu, (const struct MenuItem)ok_item));
  size_t idx = add_page(page_ctx, on_view_menu, false);
  update_result(&res, navigate_page(page_ctx, idx, menu, sizeof(*menu)));
  return res;
}